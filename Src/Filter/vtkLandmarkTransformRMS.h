#ifndef  __vtkLandmarkTransformRMS_h
#define __vtkLandmarkTransformRMS_h

#include <vtkLandmarkTransform.h>

class vtkLandmarkTransformRMS :public vtkLandmarkTransform
{
public:
	static vtkLandmarkTransformRMS* New();
    vtkTypeMacro(vtkLandmarkTransformRMS, vtkLandmarkTransform);
	virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

	void InternalUpdate() VTK_OVERRIDE
	{
		Superclass::InternalUpdate();
		this->Calculate_RMS();
	}
	

	vtkGetMacro(RMS, double);

protected:
	vtkLandmarkTransformRMS();
	virtual ~vtkLandmarkTransformRMS() VTK_OVERRIDE;

	void Calculate_RMS();


	double RMS;

};

#endif