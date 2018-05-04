#ifndef DepthIndicatorWidget_h
#define DepthIndicatorWidget_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkBorderWidget.h"

class DepthIndicatorRepresentation;

class DepthIndicatorWidget : public vtkBorderWidget
{
public:
	/**
	* Instantiate this class.
	*/
	static DepthIndicatorWidget *New();

	//@{
	/**
	* Standard VTK class methods.
	*/
	vtkTypeMacro(DepthIndicatorWidget, vtkBorderWidget);
	void PrintSelf(ostream& os, vtkIndent indent);
	//@}

	/**
	* Specify an instance of vtkWidgetRepresentation used to represent this
	* widget in the scene. Note that the representation is a subclass of vtkProp
	* so it can be added to the renderer independent of the widget.
	*/
	void SetRepresentation(DepthIndicatorRepresentation *r);


	/**
	* Create the default widget representation if one is not set.
	*/
	void CreateDefaultRepresentation();

protected:
	DepthIndicatorWidget();
	~DepthIndicatorWidget();
	
private:
	DepthIndicatorWidget(const DepthIndicatorWidget&) ;
	void operator=(const DepthIndicatorWidget&) ;
};

#endif