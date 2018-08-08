#ifndef __QVTK_BIOPSY_WIDGET_H__
#define __QVTK_BIOPSY_WIDGET_H__
#pragma once
// me 
#include "widget_export.h"
#include "QvtkWidgetOrthogonalViewer.h"
#include "QvtkUniqueUiInteractorObserver.h"
class vtkWidgetSet2;
namespace Q {
	namespace vtk {
		class Prop;
		class PolyData;
		namespace Ui { class BiopsyWidget; }
	}
}

// vtk
#include <vtkLineWidget2.h>
class vtkBoundedPlanePointPlacer;
namespace Q {
	namespace vtk {

class WIDGET_EXPORT BiopsyWidget :
	public UniqueUiInteractorObserver,
	public WidgetOrthogonalViewer,
	public vtkLineWidget2
{
	Q_OBJECT;
public:
	static const QString RADIUS;
	static const QString NORMAL;
	static const QString TAG;
	static BiopsyWidget* New();
	vtkTypeMacro(BiopsyWidget, vtkLineWidget2);
	virtual void PrintSelf(ostream& os, vtkIndent indent) override;
	Ui::BiopsyWidget* getUi() { return this->ui; }
	virtual void setCustomEnable(bool flag) override;
	virtual void install() override;
	virtual void uninstall() override;

	virtual void CreateDefaultRepresentation() override;
	vtkGetObjectMacro(WidgetSet, vtkWidgetSet2);
public slots:
	virtual void SetProjectionNormal(int normal);
	virtual void SetProjectionPosition(double x, double y, double z);
	virtual void SetRadius(double radius);

	void SetWidgetSet(vtkWidgetSet2* widgetSet);

	void SetBiopsyData(PolyData* data);


protected:
	BiopsyWidget();
	virtual ~BiopsyWidget() override;

	virtual void uniqueInstall() override;
	virtual void uniqueUninstall()override {}
	virtual void uniqueFunction()override {}
	virtual void uniqueEnable()override {}
	virtual void uniqueDisable()override {}


	static void MoveDispatcher(vtkAbstractWidget* widget);

	virtual void MoveAction(BiopsyWidget* dispatcher);

	Ui::BiopsyWidget* ui;

	vtkBoundedPlanePointPlacer* m_pointPlacer;
	vtkWidgetSet2* WidgetSet;
	PolyData* m_biopsyData;
};


	}
}
#endif // !__QVTK_BIOPSY_WIDGET_H__
