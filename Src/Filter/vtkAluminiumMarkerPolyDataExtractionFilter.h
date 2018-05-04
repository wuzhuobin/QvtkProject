 /**
 * @file	Project\code\filter\vtkAluminiumMarkerPolyDataExtraction.h
 * @language	C++
 * @author	wuzhuobin jiejin2022@163.com
 * @date	2017/12/28
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
#ifndef __VTK_ALUMINIUM_MARKER_POLY_DATA_EXTRACTION_FILTER_H__
#define __VTK_ALUMINIUM_MARKER_POLY_DATA_EXTRACTION_FILTER_H__

// vtk
#include <vtkPolyDataAlgorithm.h>
class vtkImageData;

/**
 * @class	vtkAluminiumMarkerPolyDataExtractionFilter
 * @brief	For Sucabot Dental, automatic extract a aluminium marker mesh to do ICP
 *			registration. 
 * @author	WUZHUOBIN
 * @date	2017/12/28
 *
 * This class was only applied to 2 phantom images which has an aluminium marker. It might not 
 * be appliable to real human mandible or maxilla. 
 *
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
 * 
 * In the future version, the segmentation should not be segmented from a downsample image. 
 * Perhaps K-means can help to generate an initial segmentation instead of being the final 
 * result. K-means to do rough or coarse segmentaiton to a downsample image to generate a mask. 
 * Dilating the mask a little bit and doing masking, otsu should able to differentiate gripper 
 * and aluminium marker since other irrelevant voxel have been removed. Or by knowing the volume 
 * of the aluminium marker, doing hitogram statistics the finding the volume matching.
 * Anyway, by doing masking can optimize other thresholding. 
 * 
 */

class vtkAluminiumMarkerPolyDataExtractionFilter : public vtkPolyDataAlgorithm
{
public:
	/**
	 * @fn	static vtkAluminiumMarkerPolyDataExtractionFilter* New();
	 * @brief	static vtk New() method; 
	 * @static
	 * @return	A vtkAluminiumMarkerPolyDataExtractionFilter pointer.
	 */
	static vtkAluminiumMarkerPolyDataExtractionFilter* New();
	/**
	 * @brief	vtkTypeMacro
	 */
	vtkTypeMacro(vtkAluminiumMarkerPolyDataExtractionFilter, vtkPolyDataAlgorithm);
	/**
	 * @fn	virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
	 * @brief	 vtk PrintSelf method.
	 * @override
	 * @param[in]		os
	 * @param[in]		indent
	 */
	virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

	/**
	 * @brief	Set function for #VolumeTolerance
	 * @param[in]		VolumeTolerance
	 * 
	 * Volume Tolerance is the largest tolerance of a difference between a connected component 
	 * volume and the #TargetVolume. The Volume satisfy this condition will be selected. 
	 */
	vtkSetMacro(VolumeTolerance, double);
	/**
	 * @brief	Get function for #VolumeTolerance
	 * @param[out]		VolumeTolerance
	 * 
	 * Volume Tolerance is the largest tolerance of a difference between a connected component 
	 * volume and the #TargetVolume. The Volume satisfy this condition will be selected. 
	 */
	vtkGetMacro(VolumeTolerance, double);

	/**
	 * @brief	Set function for #TargetVolume. 
	 * @param[in]		TargetVolume
	 * 
	 * Target volume is the volume of the aluminium marker(from *.stl or by calculating). 
	 */
	vtkSetMacro(TargetVolume, double);
	/**
	 * @brief	Get function for #TargetVolume. 
	 * @param[out]		TargetVolume
	 * 
	 * Target volume is the volume of the aluminium marker(from *.stl or by calculating). 
	 */
	vtkGetMacro(TargetVolume, double);


protected:
	vtkAluminiumMarkerPolyDataExtractionFilter();
	virtual ~vtkAluminiumMarkerPolyDataExtractionFilter() VTK_OVERRIDE;


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

};

#endif // !__VTK_ALUMINIUM_MARKER_POLY_DATA_EXTRACTION_FILTER_H__
