#ifndef __QVTK_IMPLANT_WIDGET_H__
#define __QVTK_IMPLANT_WIDGET_H__
#pragma once
// me
#include "widget_export.h"
#include "QvtkNoUiInteractorObserver.h"
#include "QvtkWidgetOrthogonalViewer.h"
namespace Q{
	namespace vtk {
		class Prop;
		class ImplantWidgetCameraCallback;
	}
}
// vtk 
#include <vtkAffineWidget.h>
namespace Q {
	namespace vtk {
class WIDGET_EXPORT ImplantWidget :
	public NoUiInteractorObserver,
	public WidgetOrthogonalViewer,
	public vtkAffineWidget
{
	Q_OBJECT;
public:
	static ImplantWidget* New();
	vtkTypeMacro(ImplantWidget, vtkAffineWidget);
	virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

	virtual void setCustomEnable(bool flag) override;
	
	virtual void SetEnabled(int i) VTK_OVERRIDE;

	vtkGetMacro(ImplantWidgetProp, Prop*);
	vtkSetMacro(ImplantWidgetProp, Prop*);
protected:
	ImplantWidget();
	virtual ~ImplantWidget() VTK_OVERRIDE;
	ImplantWidgetCameraCallback* CameraCallback;
	Prop* ImplantWidgetProp;
};
	}
}

#endif // !__QVTK_IMPLANT_WIDGET_H__
