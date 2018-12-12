/**
 * @file vtkPCA_ICP_Transform.h
 * @author WUZHUOBIN (jiejin2022@163.com)
 * @brief 
 * @version 0.1
 * @since Dec.12.2018
 * @copyright Copyright (c) 2018
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * &nbsp; This program is distributed in the hope that it will be useful, but	 <br>
 * &nbsp; WITHOUT ANY WARRANTY; without even the implied warranty of			 <br> 
 * &nbsp; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.				     <br> 
 * &nbsp; See the LICENSE for more detail.									     <br> 
 * &nbsp; Copyright (c) WUZHUOBIN. All rights reserved.						     <br> 
 * &nbsp; See COPYRIGHT for more detail.										 <br> 
 * &nbsp; This software is distributed WITHOUT ANY WARRANTY; without even		 <br> 
 * &nbsp; the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR	 <br> 
 * &nbsp; PURPOSE.  See the above copyright notice for more information.		 <br>
 * &nbsp; Internal usage only, without the permission of the author, please DO   <br>
 * &nbsp; NOT publish and distribute without the author's permission.  	         <br>
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
#ifndef __VTK_PCA_ICP_TRANSFORM_H__
#define __VTK_PCA_ICP_TRANSFORM_H__
#pragma once
// me 
#include "filter_export.h"
// vtk
#include <vtkIterativeClosestPointTransform.h>
class vtkMatrix4x4;
class vtkPolyData;
/**
 * @class vtkPCA_ICP_Transform
 * @brief An iterative closest point algorithm with principal component analysis.
 * @language C++
 * @author WUZHUOBIN
 * @since Dec.12.2018
 * @date Dec.12.2018 
 * 
 * vtkPCA_ICP_Transform inherints vtkIterativeClosestPointTransform with some modifications.
 * The registration is initialized with principal component analysis. It uses the center of
 * mass of target as translation initializer and the direction of principal axis as 
 * the direction. The usage of this class is exactly the same as vtkIterativeClosestPointTransform.
 * 
 */
class FILTER_EXPORT vtkPCA_ICP_Transform : public vtkIterativeClosestPointTransform
{
public:
  /**
   * @brief Factory method.
   * @return vtkPCA_ICP_Transform* 
   */
  static vtkPCA_ICP_Transform* New();
  vtkTypeMacro(vtkPCA_ICP_Transform, vtkIterativeClosestPointTransform);
  /**
   * @brief Print method. 
   * @param[in, out] os 
   * @param[in, out] indent 
   */
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
  /**
   * @brief Get RMS.
   * @return RMS
   * @see vtkPolyDataDistance
   * 
   * RMS is Root Mean Square Error. It is thd root mean square relative distance 
   * between the #TransformPolyData and vtkIterativeClosestPointTransform#Target
   * by using vtkPolyDataDistance.
   */
  vtkGetMacro(RMS, double);
  /**
   * @brief Get TransformPolyData
   * @return TransformPolyData
   * 
   *  Get the souce poly data which was applied the transformation.
   */
  vtkGetObjectMacro(TransformPolyData, vtkPolyData);
  /**
   * @brief Get SourceCenter.
   * @return  SourceCenter
   */
  vtkGetVector3Macro(SourceCenter, double);
  /**
   * @brief Get TargetCenter.
   * @return  TargetCenter
   */
  vtkGetVector3Macro(TargetCenter, double);
  /**
   * @brief Get Source Direction0.
   * @return  SourceDirection0
   */
  vtkGetVector3Macro(SourceDirection0, double);
  /**
   * @brief Get Target Direction0.
   * @return  TargetDirection0
   */
  vtkGetVector3Macro(TargetDirection0, double);
  /**
   * @brief Get Source Direction1.
   * @return  SourceDirection1
   */
  vtkGetVector3Macro(SourceDirection1, double);
  /**
   * @brief Get Target Direction1.
   * @return  TargetDirection1
   */
  vtkGetVector3Macro(TargetDirection1, double);
protected:
  /**
   * @brief Construct a new vtkPCA ICP Transform object
   * 
   */
  vtkPCA_ICP_Transform();
  /**
   * @brief Destroy the vtkPCA ICP Transform object
   * 
   */
  virtual ~vtkPCA_ICP_Transform() VTK_OVERRIDE;
  /**
   * @brief Internal update method to do PCA first. 
   * 
   */
  virtual void InternalUpdate() VTK_OVERRIDE;
  double RMS; /// <<< Root Meas Square Error between Source and Target.
  vtkPolyData* TransformPolyData; ///< The transformed source poly data.
  double SourceCenter[3]; ///< Center of mass of the source poly data.
  double TargetCenter[3]; ///< Center of mass of the target poly data.
  double SourceDirection0[3]; ///< The 1st prinicipal axis of source poly data.
  double SourceDirection1[3]; ///< The 2nd prinicipal axis of source poly data.
  double TargetDirection0[3]; ///< The 1st prinicipal axis of target poly data.
  double TargetDirection1[3]; ///< The 1st prinicipal axis of target poly data.
};

#endif // !__VTK_PCA_ICP_TRANSFORM_H__
