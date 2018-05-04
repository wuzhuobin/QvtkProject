#ifndef __QVTK_PLANAR_SEED_WIDGET_H__
#define __QVTK_PLANAR_SEED_WIDGET_H__

//me
#include "vtkPlanarSeedWidget.h"
#include "QvtkUniqueUiInteractorObserver.h"
#include "QvtkWidgetOrthogonalViewer.h"
namespace Q {
	namespace vtk {

namespace Ui { class PlanarSeedWidget; }
class PolyData;

class PlanarSeedWidget :
	public UniqueUiInteractorObserver, 
	public WidgetOrthogonalViewer, 
	public vtkPlanarSeedWidget
{
	Q_OBJECT;
public:
	static const QString SCB_PLANAR_SEED_WIDGET_DATA;

	static PlanarSeedWidget* New();
	vtkTypeMacro(PlanarSeedWidget, vtkPlanarSeedWidget);
	virtual void PrintSelf(ostream& os, vtkIndent indent) override;

	Ui::PlanarSeedWidget* getUi() { return this->ui; }
	virtual void setCustomEnable(bool flag) override;
	virtual void install() override;
	virtual void uninstall() override;

	virtual void SetEnabled(int i) override;

public slots:

	virtual void EnableLeftClckDropSeed(bool flag);
	virtual void DropSeed();
	virtual void DeleteSeed();
	virtual void DeleteSeed(int i);
	virtual void CleanAllSeed() override;
	virtual void SetFocalSeed(int i);

	virtual void SetProjectionNormal(int normal) override { 
		Superclass::SetProjectionNormal(normal); }
	virtual void SetProjectionPosition(double x, double y, double z) override;
	//{Superclass::SetProjectionPosition(x, y, z); }


	virtual void AddPointAction(vtkPlanarSeedWidget* dispatcher) override;
	virtual void MoveAction(vtkPlanarSeedWidget* dispatcher) override;
	virtual void DeleteAction(vtkPlanarSeedWidget* dispatcher) override;

	void AddIndex(unsigned int i);
	int GetIndex(unsigned int i);
	int RemoveIndex(unsigned int i);
	int IndexSize();

protected:


	explicit PlanarSeedWidget();
	virtual ~PlanarSeedWidget() override;

	virtual void uniqueInstall() override;
	virtual void uniqueUninstall()override {}
	virtual void uniqueFunction()override {}
	virtual void uniqueEnable()override {}
	virtual void uniqueDisable()override {}

	virtual void EnabledHandleInRange(vtkHandleWidget* handle);

	virtual void UpdateListWidgetSeed();
	static QString ItemTranslate(vtkIdType id, const double pos[3]);

	PolyData* SeedWidgetData;

	QList<unsigned int> m_seedIndex;

	Ui::PlanarSeedWidget* ui;

};

	}
}
#endif // !__QVTK_PLANAR_SEED_WIDGET_H__
