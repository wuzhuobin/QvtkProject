#ifndef __VTK_POLY_DATA_WIDGET_H
#define __VTK_POLY_DATA_WIDGET_H

//vtk
#include <vtkPolyDataSourceWidget.h>
class vtkPropPicker;
class vtkProperty;

class vtkPolyDataWidget : public vtkPolyDataSourceWidget
{
public:
	static vtkPolyDataWidget* New();
	vtkTypeMacro(vtkPolyDataWidget, vtkPolyDataSourceWidget);
	virtual void PrintSelf(ostream& os, vtkIndent indent) override;

	virtual void SetProp3D(vtkProp3D* prop);
	
	virtual void PlaceWidget(double x, double y, double z);
	virtual void PlaceWidget(double bounds[6]) override { 
		this->PlaceWidget((bounds[1] - bounds[0]) *0.5, (bounds[3] - bounds[2]) * 0.5, (bounds[5] - bounds[4]) * 0.5); 
	};
	void PlaceWidget()
	{
		this->Superclass::PlaceWidget();
	}
	void PlaceWidget(double xmin, double xmax, double ymin, double ymax,
		double zmin, double zmax){
		this->Superclass::PlaceWidget(xmin, xmax, ymin, ymax, zmin, zmax);
	}


	virtual vtkPolyDataAlgorithm* GetPolyDataAlgorithm() { return nullptr; }
	virtual void UpdatePlacement() override;

	virtual void SetEnabled(int enabling) override;
	virtual void Render();


	vtkGetObjectMacro(HoverProperty, vtkProperty);
	vtkGetObjectMacro(SelectedProperty, vtkProperty);

protected:
	vtkPolyDataWidget();
	virtual ~vtkPolyDataWidget() override;
	static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata);
	virtual bool FoundPickedProp(int x, int y);
	virtual void BuildRepresentation();

	virtual void StartAction(unsigned int state);
	virtual void EndAction(unsigned int state);
	virtual void Rotate();
	virtual void Spin();
	virtual void Pan();
	virtual void Dolly();
	virtual void UniformScale();

	virtual void SetPropProperty(vtkProperty* property);
	virtual void Select(bool flag);
	virtual void Hover(bool flag);

	virtual void OnLeftButtonDown();
	virtual void OnLeftButtonUp();
	virtual void OnMiddleButtonDown();
	virtual void OnMiddleButtonUp();
	virtual void OnRightButtonDown();
	virtual void OnRightButtonUp();
	virtual void OnMouseMove();

	typedef enum WidgetState
	{
		Start = 0,
		Rotating,
		Panning,
		Spinning,
		Dollying,
		UniformScaling,
	}WidgetState;
	unsigned int State;

	vtkPropPicker* PropPicker;
	vtkProperty* PropProperty;
	vtkProperty* SelectedProperty;
	vtkProperty* HoverProperty;

	double MotionFactor;
	// used in UpdatePlacement
	// cached only;
	double BoxCenter[3];
	int NumRotation;
	double Rotate2x4[2][4];
	double Scale[3];
};


#endif // !__VTK_POLY_DATA_WIDGET_H
