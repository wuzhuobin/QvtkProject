 /**
 * @file		QvtkProject\Src\Filter\vtkAMPDEFilter.h
 * @language	C++
 * @author		wuzhuobin jiejin2022@163.com
 * @date		2017/12/28
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *			This program is distributed in the hope that it will be useful, but	 *
 *			WITHOUT ANY WARRANTY; without even the implied warranty of			 * 
 *			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.				 * 
 *			See the LICENSE for more detail.									 * 
 *			Copyright (c) WUZHUOBIN. All rights reserved.						 * 
 *			See COPYRIGHT for more detail.										 * 
 *			This software is distributed WITHOUT ANY WARRANTY; without even		 * 
 *			the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR	 * 
 *			PURPOSE.  See the above copyright notice for more information.		 *
 *			Internal usage only, without the permission of the author, please DO *
 *			NOT publish and distribute without the author's permission.  	     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
#ifndef __VTK_AMPDE_FILTER_H__
#define __VTK_AMPDE_FILTER_H__
#pragma once;
// me 
#include "filter_export.h"
// vtk
#include <vtkPolyDataAlgorithm.h>
class vtkImageData;
/**
 * @class	vtkAMPDEFilter
 * @brief	For doing Aluminium-Poly-Data-Extraction, automatic extract a aluminium marker mesh to do ICP
 *			registration. 
 * @author	WUZHUOBIN
 * @date	2018/06/28	
 * @since	2017/12/28
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * This class was only applied to 2 phantom images which has an aluminium marker. It might not 
 * be appliable to real human mandible or maxilla. 
 * Artifact in CBCT can cause the aluminium marker in image to become disuniform. K-means cannot seperate
 * an uniform label. 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  
 * @date	2017/12/28
 * This class is not yet completed yet, it is not efficient. The original thought was that it 
 * used otsu threshold to extract the aluminium marker from the background. However, 
 * otsu threshld sometimes might extract the gripper with the aluminium marker at the same time. 
 * So tried multiple otsu instead. While multiple otsu thresholding could not make sure every time
 * the aluminium marker was a lebel "2". 
 * K-means can use initial mean to generate label, which could increase the probability of making 
 * aluminium marker be a label "2". 
 * By analyzing the phantom intensity(2 phantoms only), however it was not enough. 
 * Intensity of an aluminium marker was around 2300-1500. Intensity of a 3d printing plastic gripper
 * was around -100-600. Therefore, initial means was selected to -1000(CBCT background), 
 * 600(gripper), 2000(aluminium marker), 4000(artiface, noise, metal teeth?, etc). 
 * K-means was time-consuming(more than 5 minutes) algorithm to this CBCT images(short around 500*500*500). Downsampling 
 * was considered to be used to shorten the calculation time. CBCT images was downsampled to 
 * 150*150*150(scalar type not changed), the time was shorten to around 15 seconds. 
 * Hoever by downsampling, partial volume effect became dominant, which made the segmentaiton result
 * wores. Spacing and volume made the segmentation result was bigger or smaller than the original 
 * image segmentaiton result and increased the RMS of Iterative Cloest Point. 
 * After the K-means, the filter used connected component and compared each component volume to 
 * the target aluminium marker volume. The closet volume and difference between target aluminium 
 * marker volume and segmentation volume that is under tollerance was selected as the final result. 
 * @date	2018/05/15
 * K-means can help to generate an initial segmentation instead of being the final 
 * result. K-means to do rough or coarse segmentaiton to a downsample image to generate a mask. 
 * Dilating the mask a little bit and doing masking, otsu should able to differentiate gripper 
 * and aluminium marker since other irrelevant voxel have been removed.
 * @date	2018/06/28
 * According to the new phantom, the new phantom material was different from the old phantom which
 * had different intensity distribution. Since then,  vtkAMPDEFilter#ResampleSize, 
 * vtkAMPDEFilter#KMeans, vtkAMPDEFilter#AluminiMean, parameters was added to this filter. 
 * By seting above parameters, the filter can apply to different situation. 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * In the future version, the segmentation should not be segmented from a downsample image. 
 * Perhaps  Or by knowing the volume of the aluminium marker, doing hitogram statistics the finding the volume matching.
 * Anyway, by doing masking can optimize other thresholding. 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */

class FILTER_EXPORT vtkAMPDEFilter : public vtkPolyDataAlgorithm
{
public:
	/**
	 * @fn	static vtkAMPDEFilter* New();
	 * @brief	static vtk New() method; 
	 * @static
	 * @return	A vtkAMPDEFilter pointer.
	 */
	static vtkAMPDEFilter* New();
	/**
	 * @brief	vtkTypeMacro
	 */
	vtkTypeMacro(vtkAMPDEFilter, vtkPolyDataAlgorithm);
	/**
	 * @fn	virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
	 * @brief	 vtk PrintSelf method.
	 * @override
	 * @param[in]		os
	 * @param[in]		indent
	 */
	virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

	/**
	 * @brief	Set function for #VolumeTolerance.
	 * @param[in]		VolumeTolerance
	 * 
	 * Volume Tolerance is the largest tolerance of a difference between a connected component 
	 * volume and the #TargetVolume. The Volume satisfy this condition will be selected. 
	 * Default is VTK_DOUBLE_MIN.
	 */
	vtkSetMacro(VolumeTolerance, double);
	/**
	 * @brief	Get function for #VolumeTolerance. 
	 * @param[out]		VolumeTolerance
	 * 
	 * Volume Tolerance is the largest tolerance of a difference between a connected component 
	 * volume and the #TargetVolume. The Volume satisfy this condition will be selected. 
	 * Default is VTK_DOUBLE_MIN.
	 */
	vtkGetMacro(VolumeTolerance, double);

	/**
	 * @brief	Set function for #TargetVolume. 
	 * @param[in]		TargetVolume
	 * 
	 * Target volume is the volume of the aluminium marker(from *.stl or by calculating). 
	 * Default is 0;
	 */
	vtkSetMacro(TargetVolume, double);
	/**
	 * @brief	Get function for #TargetVolume. 
	 * @param[out]		TargetVolume
	 * 
	 * Target volume is the volume of the aluminium marker(from *.stl or by calculating). 
	 * Default is 0;
	 */
	vtkGetMacro(TargetVolume, double);
	/**
	 * @brief	Set resample size for doing resample. Default is [100, 100, 100]
	 * @param[in]	ResampleSize
	 * 
	 * Seting resample size to do resample before k-means algorithm, smaller faster. 
	 * However if resample size is too small, extract may be fail and partial volume effect 
	 * become significant. 
	 */
	vtkSetVector3Macro(ResampleSize, double);
	/**
	 * @brief	Get resample size for doing resample. Default is [100, 100, 100]
	 * @return	#ResampleSize
	 * 
	 * Seting resample size to do resample before k-means algorithm, smaller faster. 
	 * However if resample size is too small, extract may be fail and partial volume effect 
	 * become significant. 
	 */
	vtkGetVector3Macro(ResampleSize, double);
	/**
	 * @fn	virtual void SetKMeans(const std::vector<double>& k);
	 * @brief	Set initial mean voxel value for evey cluster in image. 
	 * @param[in]	k input for k-means. 
	 *
	 * Initial mean voxel value for different cluster in image. Valuses should be set 
	 * according to image. e.g. CBCT image. Backgound is -1000, phantom(exclude teeth) is 600, 
	 * aluminium is 2000, phanton teeth is 4000.
	 * @code SetKMeans(std::vector<double>{-1000.0, 600, 2000, 4000}); @endcode
	 * Default is [-1000, 600, 2000, 4000]. 
	 */
	virtual void SetKMeans(const std::vector<double>& k);
	/**
	 * @fn	virtual std::vector<double> GetKMeans() const;
	 * @brief	Get initial mean voxel value for evey cluster in image. 
	 * @return	#KMeans 
	 *
	 * Initial mean voxel value for different cluster in image. Valuses should be set 
	 * according to image. e.g. CBCT image. Backgound is -1000, phantom(exclude teeth) is 600, 
	 * aluminium is 2000, phanton teeth is 4000. 
	 * @code SetKMeans(std::vector<double>{-1000.0, 600, 2000, 4000}); @endcode
	 * Default is [-1000, 600, 2000, 4000]. 
	 */
	virtual std::vector<double> GetKMeans() const { return this->KMeans; }
	/**
	 * @brief	Set using which ordinal means as the aluminium mean in k menas to be extract as aluminium. 
	 * @param[in]	AluminiumMeann The oridinal in KMeans. 
	 * 
	 * Oridinal of aluminium in Kmeans. The value was used to threshold other clusters like backgound, phantom teeth. 
	 * The k means output label is in sequential, so it was clamp from 0 to #Kmeans.size().
	 * e.g. CBCT image. Backgound is -1000, phantom(exclude teeth) is 600, 
	 * aluminium is 2000, phanton teeth is 4000, AluminiumMean is 2. 
	 */
	vtkSetClampMacro(AluminiumMean, unsigned int, 0, this->KMeans.size());
	/**
	 * @brief	Set using which ordinal means as the aluminium mean in k menas to be extract as aluminium. 
	 * @param[in]	AluminiumMeann The oridinal in KMeans. 
	 * 
	 * Oridinal of aluminium in Kmeans. The value was used to threshold other clusters like backgound, phantom teeth. 
	 * The k means output label is in sequential, so it was clamp from 0 to #Kmeans.size().
	 * e.g. CBCT image. Backgound is -1000, phantom(exclude teeth) is 600, 
	 * aluminium is 2000, phanton teeth is 4000, AluminiumMean is 2. 
	 */
	vtkGetMacro(AluminiumMean, unsigned int);

protected:
	/**
	 * @brief	Constructor. 
	 */
	vtkAMPDEFilter();
	/**
	 * @brief	Destructor.
	 */
	virtual ~vtkAMPDEFilter() VTK_OVERRIDE;


	/**
	 * @brief	vtk pipeline method. 
	 * @override
	 */
	virtual int RequestData(
		vtkInformation *request, 
		vtkInformationVector ** inputVector, 
		vtkInformationVector * outputVector) VTK_OVERRIDE;
	/**
	 * @brief	Make the 1st input port to be vtkImageData. 
	 * @override
	 * @param[in]	port 
	 * @param[in]	info
	 * @return int
	 */
	virtual int FillInputPortInformation(int port, vtkInformation *info) VTK_OVERRIDE;

	/**
	 * @fn 	template<typename ScalarType>
	 *		int ITK_Calculation(vtkImageData* input, vtkImageData* output);
	 * @brief using itk filter to do calculation. 
	 * @tpatam			ScalarType	the scalar type of the vtkImageData, for using vtkTypeMacro.
	 * @param[in]		input input image. 
	 * @param[out]		output output image. 
	 */
	template<typename ScalarType>
	int ITK_Calculation(vtkImageData* input, vtkImageData* output);

	double VolumeTolerance; ///<  Volume Tolerance is the largest tolerance of a difference between a connected component volume and the #TargetVolume. The Volume satisfy this condition will be selected.
	double TargetVolume;	///<	Target volume is the volume of the aluminium marker(from *.stl or by calculating). 
	double ResampleSize[3]; ///<	The resample size for k-means for increasing speed. 
	std::vector<double> KMeans; ///<	The size of KMeans is K, the values are means for K-Means algorithm.
	unsigned int AluminiumMean; ///<	The ordinal of aluminium in kmeans. 
private:
	vtkAMPDEFilter(const vtkAMPDEFilter&) VTK_DELETE_FUNCTION;
	void operator=(const vtkAMPDEFilter&) VTK_DELETE_FUNCTION;
};

#endif // !__VTK_AMPDE_FILTER_H__
