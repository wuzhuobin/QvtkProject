/**
* @file	Project\code\filter\vtkMrfKmeansImageSegmentationFilter.h
* @language	C++
* @author	jacky ko jackkykokoko@gmail.com
* @date	2017/12/28
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*			This program is distributed in the hope that it will be useful, but	 *
*			WITHOUT ANY WARRANTY; without even the implied warranty of			 *
*			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.				 *
*			See the LICENSE for more detail.									 *
*			Copyright (c) Jacky Ko. All rights reserved.						 *
*			See COPYRIGHT for more detail.										 *
*			This software is distributed WITHOUT ANY WARRANTY; without even		 *
*			the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR	 *
*			PURPOSE.  See the above copyright notice for more information.		 *
*			Internal usage only, without the permission of the author, please DO *
*			NOT publish and distribute without the author's permission.  	     *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

#ifndef VTKMRFKMEANSIMAGESEGMENTATIONFILTER_H
#define VTKMRFKMEANSIMAGESEGMENTATIONFILTER_H
#pragma once;
// me 
#include "filter_export.h"
// vtk
#include "vtkImageAlgorithm.h"
/**
* @class	vtkMrfKmeansImageSegmentatioinFilter
* @brief	For Sucabot Dental, automatic segmentation of teeth boy.
* @author	Jacky Ko
* @date	2018/3/5
*
*
*/

class vtkDoubleArray;

class FILTER_EXPORT vtkMrfKmeansImageSegmentationFilter : public vtkImageAlgorithm
{
public:
	/**
	* @fn	static vtkMrfKeamsImageSegmentationFilter* New();
	* @brief	static vtk New() method;
	* @static
	* @return	A vtkMrfKeamsImageSegmentationFilter pointer.
	*/
	static vtkMrfKmeansImageSegmentationFilter *New();
	/**
	* @brief	vtkTypeMacro
	*/
	vtkTypeMacro(vtkMrfKmeansImageSegmentationFilter, vtkAlgorithm);
	/**
	* @fn	virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
	* @brief	 vtk PrintSelf method.
	* @override
	* @param[in]		os
	* @param[in]		indent
	*/
	virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

	/**
	* @brief	Set function for #NumberOfKmeansInitClass
	* @param[in]		NumberOfKmeansInitClass
	*
	* Kmeans clustering request for number of cluster classes for algorithm initiation.
	*/
	vtkSetMacro(NumberOfKmeansInitClass, unsigned int);

	/**
	* @brief	Get function for #NumberOfKmeansInitClass
	* @param[in]		NumberOfKmeansInitClass
	*
	* Kmeans clustering request for number of cluster classes for algorithm initiation.
	*/
	vtkGetMacro(NumberOfKmeansInitClass, unsigned int);

	/**
	* @brief	Set function for #UseNonContinguousLabels
	* @param[in]		NumberOfKmeansInitClass
	*
	* Use non continguous labels during kmeans clustering.;
	*/
	vtkSetMacro(UseNonContinguousLabels, bool);

	/**
	* @brief	Get function for #UseNonContinguousLabels
	* @param[in]		NumberOfKmeansInitClass
	*
	* Use non continguous labels during kmeans clustering.;
	*/
	vtkGetMacro(UseNonContinguousLabels, bool);

	/**
	* @brief	Get function for #EstimatedKmeansValue
	* @param[in]		EstimatedKmeansValue
	*
	* Estimated means computed by kmeans filter;
	*/
	vtkGetMacro(EstimatedKmeansValues, vtkDoubleArray*);

	/**
	* @brief	Set function for #NumberOfMaxMrfIterations
	* @param[in]		NumberOfMaxMrfIterations
	*
	* Number of maxiumum iterations used in Markov Random Field Segmentation. (default = 20);
	*/
	vtkSetMacro(NumberOfMaxMrfIterations, unsigned int);

	/**
	* @brief	Get function for #NumberOfMaxMrfIterations
	* @param[in]		NumberOfMaxMrfIterations
	*
	* UNumber of maxiumum iterations used in Markov Random Field Segmentation. (default = 20);
	*/
	vtkGetMacro(NumberOfMaxMrfIterations, unsigned int);

	/**
	* @brief	Set function for #MrfSmoothingFactor
	* @param[in]		MrfSmoothingFactor
	*
	* Number of smoothiing factor used in Markov Random Field Segmentation. (default = 2);
	*/
	vtkSetMacro(MrfSmoothingFactor, double);

	/**
	* @brief	Get function for #MrfSmoothingFactor
	* @param[in]		MrfSmoothingFactor
	*
	* UNumber of smoothing factor used in Markov Random Field Segmentation. (default = 2);
	*/
	vtkGetMacro(MrfSmoothingFactor, double);

protected:
	vtkMrfKmeansImageSegmentationFilter();
	virtual ~vtkMrfKmeansImageSegmentationFilter() VTK_OVERRIDE;

	/**
	* @brief	vtk pipeline method.
	* @override
	*/
	int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
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
	template<typename PixelType>
	int ITK_Calculation(vtkImageData* input, vtkImageData* output);

	unsigned int NumberOfKmeansInitClass; ///< Number of class labels for kmeans initialization.
	bool UseNonContinguousLabels; ///< Use non continguous labels during kmeans segmentation
	vtkDoubleArray* EstimatedKmeansValues;
	unsigned int NumberOfMaxMrfIterations;
	double MrfSmoothingFactor;

private:
	vtkMrfKmeansImageSegmentationFilter(const vtkMrfKmeansImageSegmentationFilter&);
	void operator =(const vtkMrfKmeansImageSegmentationFilter&);

};

#endif