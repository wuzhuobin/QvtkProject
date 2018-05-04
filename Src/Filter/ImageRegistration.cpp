#include "ImageRegistration.h"

#include "itkVersorRigid3DTransformOptimizer.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageRegistrationMethod.h"

typedef itk::VersorRigid3DTransformOptimizer									OptimizerType; 		//Optimizer
typedef OptimizerType::ScalesType												OptimizerScalesType;
typedef itk::MattesMutualInformationImageToImageMetric<ImageRegistration::ImageType, ImageRegistration::ImageType>	MetricType;			//Metric
typedef itk::LinearInterpolateImageFunction <ImageRegistration::ImageType, double>					InterpolatorType;	//interpolation
typedef itk::ImageRegistrationMethod <ImageRegistration::ImageType, ImageRegistration::ImageType>						RegistrationType;	//registration method

ImageRegistration::ImageRegistration(QObject* parent) : QObject(parent)
{
	m_fixedImage	 = NULL;
	m_movingImage	 = NULL;
	m_finalTransform = NULL;
	m_outputImage	 = NULL;
}

ImageRegistration::~ImageRegistration()
{
}

void ImageRegistration::SetInitialMatrix(itk::Matrix<double, 3, 3> matrix)
{
	m_initialMatrix = matrix;
}

void ImageRegistration::SetInitialRotation(double rotX, double rotY, double rotZ)
{
	m_initialRotation.SetElement(0, rotX);
	m_initialRotation.SetElement(1, rotY);
	m_initialRotation.SetElement(2, rotZ);
}

void ImageRegistration::SetInitialTranslation(double transX, double transY, double transZ)
{
	m_initialTranslation.SetElement(0, transX);
	m_initialTranslation.SetElement(1, transY);
	m_initialTranslation.SetElement(2, transZ);
}

void ImageRegistration::SetInitialCenter(double centerX, double centerY, double centerZ)
{
	m_initialCenter.SetElement(0, centerX);
	m_initialCenter.SetElement(1, centerY);
	m_initialCenter.SetElement(2, centerZ);
}

void ImageRegistration::SetFixedImage( ImageType::Pointer fixedImage )
{
	m_fixedImage = fixedImage;	
}

void ImageRegistration::SetMovingImage( ImageType::Pointer movingImage )
{
	m_movingImage = movingImage;
}

bool ImageRegistration::Update()
{
	std::cout << "registration starts" << std::endl;
	if (!m_fixedImage| !m_movingImage)
		return EXIT_FAILURE;

	//initialization
	TransformType::Pointer		transform	  =	TransformType::New();
	MetricType::Pointer         metric        = MetricType::New();
	OptimizerType::Pointer      optimizer     = OptimizerType::New();
	InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
	////InitializerType::Pointer    initializer	  = InitializerType::New();
	RegistrationType::Pointer   registration  = RegistrationType::New();

	//link metric, optimizer, interpolator and transform to registration
	metric->SetNumberOfSpatialSamples(100000);

	registration->SetMetric(metric);
	registration->SetOptimizer(optimizer);
	registration->SetInterpolator(interpolator);
	registration->SetTransform(transform);

	//set the fixed and moving images
	registration->SetFixedImage(m_fixedImage);
	registration->SetMovingImage(m_movingImage);
	registration->SetFixedImageRegion(m_fixedImage->GetBufferedRegion());

	//Initialize transform
	std::cout << "initial 3x3 matrix" << std::endl;
	std::cout << m_initialMatrix << std::endl;

	transform->SetMatrix(m_initialMatrix);

	std::cout << "initial translation" << std::endl;
	std::cout << m_initialTranslation[0] << "," << m_initialTranslation[1] << "," << m_initialTranslation[2] << std::endl;
	transform->SetTranslation(m_initialTranslation);

	std::cout << "initial center" << std::endl;
	std::cout << m_initialCenter[0] << "," << m_initialCenter[1] << "," << m_initialCenter[2] << std::endl;
	transform->SetCenter(m_initialCenter);

	std::cout << "initial transform" << std::endl;
	transform->Print(std::cout);

	//registration get the initial transform parameters
	registration->SetInitialTransformParameters(transform->GetParameters());

	//set the optimizer
	OptimizerScalesType optimizerScales(transform->GetNumberOfParameters());
	double translationScale = 1.0 /1000.0;
	optimizerScales[0] = 1.0;
	optimizerScales[1] = 1.0;
	optimizerScales[2] = 1.0;
	optimizerScales[3] = translationScale;
	optimizerScales[4] = translationScale;
	optimizerScales[5] = translationScale;

	optimizer->SetScales(optimizerScales);

	//set step length and iterations
	optimizer->SetMaximumStepLength(0.2000);
	//optimizer->SetMaximumStepLength(0.005);
	optimizer->SetMinimumStepLength(0.001);
	optimizer->SetNumberOfIterations(1500);
	optimizer->SetRelaxationFactor(0.5);

	//print out error messages if the registration fails
	try
	{
		registration->Update();
		std::cout << "Optimizer stop condition:"
			<< registration->GetOptimizer()->GetStopConditionDescription()
			<<std::endl;
	}
	catch (itk::ExceptionObject &err)
	{
		std::cerr << "ExceptionObject caught!" << std::endl;
		std::cerr << err << std::endl;
		return EXIT_FAILURE;
	}

	const TransformType::ParametersType finalParameters = registration->GetLastTransformParameters();

	//get the result of the optimizer
	TransformType::Pointer finalTransform = TransformType::New();
	transform->SetParameters(finalParameters);
	finalTransform->SetCenter(transform->GetCenter());  //for affine transform
	finalTransform->SetParameters(finalParameters);
	finalTransform->SetFixedParameters(transform->GetFixedParameters());

	std::cout << "final transform" << std::endl;
	finalTransform->Print(std::cout);

	//reserve the finalTransform
	m_finalTransform = finalTransform;

	//return the registered image
	return EXIT_SUCCESS;
}

ImageRegistration::TransformType::Pointer ImageRegistration::GetOutputTransform()
{
	if (m_finalTransform)
	{
		return m_finalTransform;
	}
	
	return NULL;
}

ImageRegistration::ImageType::PointType ImageRegistration::GetOutputOrigin()
{
	return m_outputImage->GetOrigin();
	return NULL;
}

ImageRegistration::ImageType::DirectionType ImageRegistration::GetOutputDirection()
{
	return m_outputImage->GetDirection();
}