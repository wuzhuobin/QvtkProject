#ifndef __QVTK_POLY_DATA_SOURCE_WIDGET_H__
#define __QVTK_POLY_DATA_SOURCE_WIDGET_H__

// me
#include "QvtkUniqueUiInteractorObserver.h"
#include "QvtkWidgetOrthogonalViewer.h"
#include "vtkPolyDataWidget.h"
namespace Q {
	namespace vtk {
		class Prop;
		class PolyData;
		class AnnotationPolyData;
		namespace Ui { class PolyDataSourceWidget; };
	}
}
// vtk
class vtkPropPicker;
namespace Q {
	namespace vtk {
class PolyDataSourceWidget :
	public UniqueUiInteractorObserver,
	public WidgetOrthogonalViewer,
	public vtkPolyDataWidget
{
	Q_OBJECT;
public:
	static PolyDataSourceWidget* New();
	vtkTypeMacro(PolyDataSourceWidget, vtkPolyDataWidget);
	virtual void PrintSelf(ostream& os, vtkIndent indent) override;
	Ui::PolyDataSourceWidget* getUi() { return this->ui; }
	virtual void setCustomEnable(bool flag) override;
	virtual void install() override;
	virtual void uninstall() override;


	virtual void Render();
	virtual void SetPolyData(PolyData* polydata) { this->PolyData = polydata; }
	virtual PolyData* GetPolyData() { return this->PolyData; }
public slots:
	static void DisableFollowCursor(bool flag);

signals:
	void DisabledFollowCursor(bool flag);

protected:

	PolyDataSourceWidget();
	virtual ~PolyDataSourceWidget() override;
	virtual void uniqueInstall() override;
	virtual void uniqueUninstall()override {}
	virtual void uniqueFunction()override {}
	virtual void uniqueEnable()override {}
	virtual void uniqueDisable()override {}
	virtual void BuildRepresentation() override {}


	virtual void Rotate() override;
	virtual void Spin() override;
	virtual void Pan() override;
	virtual void Dolly() override;
	virtual void UniformScale() override;


	virtual void Select(bool flag) override;
	virtual void Hover(bool flag) override;

	virtual void FollowCursor();

	Prop* Prop;
	PolyData* PolyData;
	static bool m_unfollowCursorFlag;
	//AnnotationPolyData* AnnotationPolyData;
	Ui::PolyDataSourceWidget* ui;
private:

};

	}
}
#endif // !__QVTK_POLY_DATA_SOURCE_WIDGET_H__
