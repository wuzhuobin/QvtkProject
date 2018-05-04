#include "vtkMrfKmeansImageSegmentationFilter.h"

// vtk
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkSmartPointer.h"
#include "vtkDoubleArray.h"

// itk
#include "itkImage.h"
#include "itkVTKImageToImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkScalarImageKmeansImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkComposeImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkMRFImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkResampleImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include <itkLabelStatisticsImageFilter.h>

#include "itkImageFileWriter.h"

vtkStandardNewMacro(vtkMrfKmeansImageSegmentationFilter);

vtkMrfKmeansImageSegmentationFilter::vtkMrfKmeansImageSegmentationFilter()
{
	this->NumberOfKmeansInitClass = 1;
	this->UseNonContinguousLabels = true;
	this->EstimatedKmeansValues = vtkDoubleArray::New();
	this->NumberOfMaxMrfIterations = 20;
	this->MrfSmoothingFactor = 2;
}

vtkMrfKmeansImageSegmentationFilter::~vtkMrfKmeansImageSegmentationFilter()
{
	this->EstimatedKmeansValues->Delete();
}

int vtkMrfKmeansImageSegmentationFilter::RequestData(
	vtkInformation* vtkNotUsed(request),
	vtkInformationVector **inputVector,
	vtkInformationVector *outputVector)
{
	// Get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Get the input and ouptut
	vtkImageData *input = vtkImageData::SafeDownCast(
		inInfo->Get(vtkDataObject::DATA_OBJECT()));

	vtkImageData *output = vtkImageData::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	int failed = 1;
	switch (input->GetScalarType())
	{
		vtkTemplateMacro(failed= this->ITK_Calculation<VTK_TT>(input, output));
	}

	if (failed)
	{
		vtkErrorMacro(<< "ITK_Calcuation failed");
		return 1;
	}

	//vtkSmartPointer<vtkImageData> image =
	//	vtkSmartPointer<vtkImageData>::New();
	//image->ShallowCopy(input);

	//image->SetScalarComponentFromDouble(0, 0, 0, 0, 5.0);

	//output->ShallowCopy(image);

	// Without these lines, the output will appear real but will not work as the input to any other filters
	int extent[6];
	input->GetExtent(extent);
	output->SetExtent(extent);
	outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
		extent,
		6);
	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
		extent,
		6);
	return 1;
}

int vtkMrfKmeansImageSegmentationFilter::FillInputPortInformation(int port, vtkInformation * info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
	return 1;
}

void vtkMrfKmeansImageSegmentationFilter::PrintSelf(ostream & os, vtkIndent indent)
{
	Superclass::PrintSelf(os, indent);
}

template<typename PixelType>
int vtkMrfKmeansImageSegmentationFilter::ITK_Calculation(vtkImageData* input, vtkImageData* output)
{
	std::cout << "computation start..." << std::endl;

	//typedef short PixelType;
	const unsigned short DIMENSION = 3;
	typedef itk::Image<PixelType, DIMENSION> ImageType;
	typedef itk::Image<short, DIMENSION> LabelImageType; // user short here as segmentation number of labels may exceed 256
	
	typedef itk::VTKImageToImageFilter<ImageType> VtkImageToImageFilterType;
	typename VtkImageToImageFilterType::Pointer vtkImageToImageFilter = VtkImageToImageFilterType::New();
	vtkImageToImageFilter->SetInput(input);
	vtkImageToImageFilter->Update();

	vtkImageToImageFilter->GetOutput()->Print(std::cout);

	std::cout << "convert to itk image data complete..." << std::endl;

	// rescale image from 0 to 255
	typedef itk::RescaleIntensityImageFilter<ImageType, ImageType> RescaleIntensityImageFilterType;
	typename RescaleIntensityImageFilterType::Pointer intensityRescaler = RescaleIntensityImageFilterType::New();
	intensityRescaler->SetOutputMinimum(0);
	intensityRescaler->SetOutputMaximum(255);
	intensityRescaler->SetInput(vtkImageToImageFilter->GetOutput());
	intensityRescaler->Update();

	// down sample to speed up computation speed
	typename ImageType::SizeType outputSize;
	outputSize.SetElement(0, vtkImageToImageFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0] / 2);
	outputSize.SetElement(1, vtkImageToImageFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[1] / 2);
	outputSize.SetElement(2, vtkImageToImageFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[2] / 2);

	typename ImageType::SpacingType outputSpacing;
	outputSpacing[0] = static_cast<double>(vtkImageToImageFilter->GetOutput()->GetSpacing()[0]) * (static_cast<double>(vtkImageToImageFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0]) / static_cast<double>(outputSize[0]));
	outputSpacing[1] = static_cast<double>(vtkImageToImageFilter->GetOutput()->GetSpacing()[1]) * (static_cast<double>(vtkImageToImageFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[1]) / static_cast<double>(outputSize[1]));
	outputSpacing[2] = static_cast<double>(vtkImageToImageFilter->GetOutput()->GetSpacing()[2]) * (static_cast<double>(vtkImageToImageFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[2]) / static_cast<double>(outputSize[2]));

	typedef itk::IdentityTransform<double,3> TransformType;
	typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleImageFilterType;
	typename ResampleImageFilterType::Pointer downSampler = ResampleImageFilterType::New();
	downSampler->SetInput(intensityRescaler->GetOutput());
	downSampler->SetSize(outputSize);
	downSampler->SetOutputSpacing(outputSpacing);
	downSampler->SetTransform(TransformType::New());
	downSampler->UpdateLargestPossibleRegion();
	downSampler->Update();

	downSampler->GetOutput()->Print(std::cout);

	std::cout << "kmeans segmentation start..." << std::endl;

	// scalar kmeans
	typedef itk::ScalarImageKmeansImageFilter<ImageType, ImageType> ScalarImageKmeansImageFilterType;
	typename ScalarImageKmeansImageFilterType::Pointer kmeansFilter = ScalarImageKmeansImageFilterType::New();
	kmeansFilter->SetInput(downSampler->GetOutput());
	kmeansFilter->SetUseNonContiguousLabels(this->UseNonContinguousLabels);
	for (int i = 0; i < this->NumberOfKmeansInitClass; i++)
	{
		kmeansFilter->AddClassWithInitialMean(255.0 / NumberOfKmeansInitClass*(i + 1));
	}
	kmeansFilter->Update();

	typename ScalarImageKmeansImageFilterType::ParametersType estimatedMeans = kmeansFilter->GetFinalMeans();
	const unsigned int numberOfClasses = estimatedMeans.Size();

	std::cout << "number of kmenas estimated classes = " << numberOfClasses << std::endl;

	EstimatedKmeansValues->SetNumberOfTuples(numberOfClasses);
	EstimatedKmeansValues->SetNumberOfComponents(1);

	for (int i = 0; i < numberOfClasses; i++)
	{
		std::cout << "cluster[" << i << "] ";
		std::cout << "    estimated mean : " << estimatedMeans[i] << std::endl;
		float tuple[1];
		tuple[0] = (float)estimatedMeans[i];
		EstimatedKmeansValues->SetTuple(i, tuple);
	}

	std::cout << "kmeans classification complete..." << std::endl;

	// thresholding based on kmeans value
	typedef itk::BinaryThresholdImageFilter<ImageType, LabelImageType> BinaryThresholdingImageFilterType;
	typename BinaryThresholdingImageFilterType::Pointer binaryThresholder = BinaryThresholdingImageFilterType::New();
	binaryThresholder->SetInput(intensityRescaler->GetOutput());
	binaryThresholder->SetInsideValue(1);
	binaryThresholder->SetOutsideValue(0);
	binaryThresholder->SetLowerThreshold(estimatedMeans[numberOfClasses-2]);
	binaryThresholder->SetUpperThreshold(itk::NumericTraits<PixelType>::max());
	binaryThresholder->Update();

	// find the largest connected components
	typedef itk::ConnectedComponentImageFilter<LabelImageType, LabelImageType> ConnectedComponentImageFilterType;
	ConnectedComponentImageFilterType::Pointer ccFilter = ConnectedComponentImageFilterType::New();
	ccFilter->SetInput(binaryThresholder->GetOutput());
	ccFilter->SetFullyConnected(true);
	ccFilter->Update();

	typedef itk::LabelStatisticsImageFilter<LabelImageType, LabelImageType> LabelStatisticFilterType;
	LabelStatisticFilterType::Pointer labelStatFilter = LabelStatisticFilterType::New();
	labelStatFilter->SetInput(binaryThresholder->GetOutput());
	labelStatFilter->SetLabelInput(ccFilter->GetOutput());
	labelStatFilter->Update();

	typedef itk::BinaryThresholdImageFilter<LabelImageType, LabelImageType> BinaryThresholdingLabelImageFilterType;
	BinaryThresholdingLabelImageFilterType::Pointer labelThresholder = BinaryThresholdingLabelImageFilterType::New();
	labelThresholder->SetInput(ccFilter->GetOutput());
	labelThresholder->SetInsideValue(1);
	labelThresholder->SetOutsideValue(0);

	int area = 0;
	for (int i = 1; i <= labelStatFilter->GetNumberOfLabels(); i++)
	{
		if (labelStatFilter->GetCount(i) > area)
		{
			area = labelStatFilter->GetCount(i);
			labelThresholder->SetLowerThreshold(i);
			labelThresholder->SetUpperThreshold(i);
		}
	}

	labelThresholder->Update();

	typedef itk::CastImageFilter<LabelImageType, ImageType> kmeansOutputCastImageType;
	typename kmeansOutputCastImageType::Pointer kmeansCast = kmeansOutputCastImageType::New();
	kmeansCast->SetInput(labelThresholder->GetOutput());
	kmeansCast->Update();

	//std::cout << "kmeans label cast complete..." << std::endl;

	//// Image gradient magnitude
	//typedef itk::Image<float> FloatImageType;

	//typedef itk::CurvatureAnisotropicDiffusionImageFilter<ImageType, FloatImageType> SmoothingFilterType;
	//typename SmoothingFilterType::Pointer smoothFilter = SmoothingFilterType::New();
	//smoothFilter->SetTimeStep(0.010);
	//smoothFilter->SetNumberOfIterations(10);
	//smoothFilter->SetConductanceParameter(9.0);
	//smoothFilter->SetInput(downSampler->GetOutput());
	//smoothFilter->Update();

	//std::cout << "image smoothing complete..." << std::endl;

	//typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<FloatImageType, FloatImageType> gradientRecursiveType;
	//gradientRecursiveType::Pointer gradientRecursive = gradientRecursiveType::New();
	//const double sigma = 1.0;//0.5
	//gradientRecursive->SetSigma(sigma);
	//gradientRecursive->SetInput(smoothFilter->GetOutput());
	//gradientRecursive->Update();

	//std::cout << "gradient computation complete..." << std::endl;

	//typedef itk::SigmoidImageFilter<FloatImageType, ImageType> sigmoidImageType;
	//sigmoidImageType::Pointer sigmoid = sigmoidImageType::New();
	//sigmoid->SetInput(gradientRecursive->GetOutput());
	//sigmoid->SetOutputMaximum(255);
	//sigmoid->SetOutputMinimum(0);
	//sigmoid->SetAlpha(-10);
	//sigmoid->SetBeta(1);
	//sigmoid->Update();

	//std::cout << "mrf data preparation complete..." << std::endl;

	//// MRF segmentation
	//typedef itk::CovariantVector<PixelType, 2> ArrayPixelType;
	//typedef itk::Image<ArrayPixelType, 3> ArrayImageType;

	//typedef itk::ComposeImageFilter<ImageType, ArrayImageType > ScalarToArrayFilterType;
	//ScalarToArrayFilterType::Pointer scalarToArrayFilter = ScalarToArrayFilterType::New();
	//scalarToArrayFilter->SetInput(0, kmeansCast->GetOutput()); // k means binary label
	//scalarToArrayFilter->SetInput(1, downSampler->GetOutput()); // downsampled image
	////scalarToArrayFilter->SetInput(1, sigmoid->GetOutput());
	//scalarToArrayFilter->Update();

	////std::cout << "mrf vector image complete..." << std::endl;

	//typedef itk::MRFImageFilter<ArrayImageType, LabelImageType> MRFFilterType;
	//MRFFilterType::Pointer mrfFilter = MRFFilterType::New();
	//mrfFilter->SetInput(scalarToArrayFilter->GetOutput());
	//mrfFilter->SetNumberOfClasses(numberOfClasses);
	//mrfFilter->SetMaximumNumberOfIterations(this->NumberOfMaxMrfIterations);
	//mrfFilter->SetErrorTolerance(1e-7);
	//mrfFilter->SetSmoothingFactor(this->MrfSmoothingFactor);
	//mrfFilter->SetNeighborhoodRadius(1);

	//typedef itk::ImageClassifierBase<ArrayImageType, LabelImageType >   SupervisedClassifierType;
	//SupervisedClassifierType::Pointer classifier = SupervisedClassifierType::New();

	//typedef itk::Statistics::MinimumDecisionRule DecisionRuleType;
	//DecisionRuleType::Pointer  classifierDecisionRule = DecisionRuleType::New();
	//classifier->SetDecisionRule(classifierDecisionRule.GetPointer());

	//typedef itk::Statistics::DistanceToCentroidMembershipFunction<ArrayPixelType > MembershipFunctionType;
	//typedef MembershipFunctionType::Pointer MembershipFunctionPointer;
	//double meanDistance = 0;
	//MembershipFunctionType::CentroidType centroid(2);

	//for (unsigned int i = 0; i < numberOfClasses; i++)
	//{
	//	MembershipFunctionPointer membershipFunction = MembershipFunctionType::New();
	//	centroid[0] = 1;
	//	centroid[1] = estimatedMeans[i];
	//	membershipFunction->SetCentroid(centroid);
	//	classifier->AddMembershipFunction(membershipFunction);
	//	meanDistance += static_cast<double> (centroid[0]);
	//}

	//if (numberOfClasses > 0)
	//{
	//	meanDistance /= this->EstimatedKmeansValues->GetNumberOfTuples();
	//}
	//else
	//{
	//	std::cerr << "ERROR: numberOfClasses is 0" << std::endl;
	//	return EXIT_FAILURE;
	//}

	//// neighborhood weighted kernels
	//std::vector< double > weights;
	//weights.push_back(1.3);
	//weights.push_back(1.3);
	//weights.push_back(1.3);

	//weights.push_back(1.7);
	//weights.push_back(1.7);
	//weights.push_back(1.7);

	//weights.push_back(1.3);
	//weights.push_back(1.3);
	//weights.push_back(1.3);

	//weights.push_back(1.3);
	//weights.push_back(1.5);
	//weights.push_back(1.3);

	//weights.push_back(1.7);
	//weights.push_back(0);	// This is the central pixel
	//weights.push_back(1.7);

	//weights.push_back(1.5);
	//weights.push_back(1.5);
	//weights.push_back(1.3);

	//weights.push_back(1.3);
	//weights.push_back(1.3);
	//weights.push_back(1.3);

	//weights.push_back(1.7);
	//weights.push_back(1.7);
	//weights.push_back(1.7);

	//weights.push_back(1.3);
	//weights.push_back(1.3);
	//weights.push_back(1.3);

	//// We now scale weights so that the smoothing function and the image fidelity
	//// functions have comparable value. This is necessary since the label
	//// image and the input image can have different dynamic ranges. The fidelity
	//// function is usually computed using a distance function, such as the
	//// \doxygen{DistanceToCentroidMembershipFunction} or one of the other
	//// membership functions. They tend to have values in the order of the means
	//// specified.

	//double totalWeight = 0;
	//for (std::vector< double >::const_iterator wcIt = weights.begin();
	//	wcIt != weights.end(); ++wcIt)
	//{
	//	totalWeight += *wcIt;
	//}
	//for (std::vector< double >::iterator wIt = weights.begin();
	//	wIt != weights.end(); ++wIt)
	//{
	//	*wIt = static_cast< double > ((*wIt) * meanDistance / (2 * totalWeight));
	//}
	//mrfFilter->SetMRFNeighborhoodWeight(weights);

	//// Finally, the classifier class is connected to the Markof Random Fields filter.
	//std::cout << "mrf classification start..." << std::endl;

	//mrfFilter->SetClassifier(classifier);
	//mrfFilter->Update();


	////mrfOutput->Print(std::cout);

	//BinaryThresholdingLabelImageFilterType::Pointer labelThresholder2 = BinaryThresholdingLabelImageFilterType::New();
	//labelThresholder2->SetInput(mrfFilter->GetOutput());
	//labelThresholder2->SetInsideValue(1);
	//labelThresholder2->SetOutsideValue(0);
	//labelThresholder2->SetLowerThreshold(numberOfClasses-2);
	//labelThresholder2->SetUpperThreshold(itk::NumericTraits<short>::max());
	//labelThresholder2->Update();

	//// output of mrffilter is currently label image type, cast to image type
	//kmeansCast->SetInput(labelThresholder2->GetOutput());
	//kmeansCast->Update();

	//std::cout << "mrf classification complete..." << std::endl;

	////// up sample for image output
	//ImageType::Pointer mrfOutput = ImageType::New();
	//mrfOutput->Graft(kmeansCast->GetOutput());
	//mrfOutput->SetSpacing(downSampler->GetOutput()->GetSpacing());

	//// up sample image to match original image size
	//ResampleImageFilterType::Pointer upSampler = ResampleImageFilterType::New();
	//upSampler->SetInput(sigmoid->GetOutput());
	//upSampler->SetSize(vtkImageToImageFilter->GetOutput()->GetLargestPossibleRegion().GetSize());
	//upSampler->SetOutputSpacing(vtkImageToImageFilter->GetOutput()->GetSpacing());
	//upSampler->SetTransform(TransformType::New());
	//upSampler->UpdateLargestPossibleRegion();
	//upSampler->Update();

	////upSampler->GetOutput()->Print(std::cout);

	//std::cout << "upsampling complete..." << std::endl;

	// convert mrf ouput to vtk image type
	typedef itk::ImageToVTKImageFilter<ImageType> ImageToVtkImageFilterType;
	typename ImageToVtkImageFilterType::Pointer connector = ImageToVtkImageFilterType::New();
	connector->SetInput(kmeansCast->GetOutput());
	connector->Update();

	output->ShallowCopy(connector->GetOutput());

	// save the image output

	std::cout << "exporting result..." << std::endl;

	typename itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
	writer->SetInput(kmeansCast->GetOutput());
	writer->SetFileName("D:/projects/SucabotProject/build/ImageData/1/nii/seg.nii.gz");
	writer->Write();
	
	std::cout << "kmeans mrf complete!" << std::endl;

	return 0;
}

