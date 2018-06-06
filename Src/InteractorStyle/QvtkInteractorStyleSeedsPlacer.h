#ifndef __QVTK_INTERACTOR_STYLE_SEEDS_PLACER_H__
#define __QVTK_INTERACTOR_STYLE_SEEDS_PLACER_H__

#include "QvtkInteractorStyleNavigation.h"
namespace Q {
	namespace vtk {
		namespace Ui { class InteractorStyleSeedsPlacer; }
		class PolyData;
	}
}

// vtk
class vtkSeedWidget;
class vtkBoundedPlanePointPlacer;

namespace Q {
	namespace vtk {

class InteractorStyleSeedsPlacer : public InteractorStyleNavigation
{
	Q_OBJECT;
public:
	static const QString SEEDS_PLACER_DATA;
	static InteractorStyleSeedsPlacer* New();
	vtkTypeMacro(InteractorStyleSeedsPlacer, InteractorStyleNavigation);
	virtual void PrintSelf(ostream& os, vtkIndent indent) override;

	virtual void setCustomEnable(bool flag) override;
	virtual void install() override;
	virtual void uninstall() override;
	virtual void SetInteractor(vtkRenderWindowInteractor* interactor);

	//virtual void OnChar() override;

	Ui::InteractorStyleSeedsPlacer* getUi() { return this->ui; }
public slots:
	virtual void SetCursorPosition(double x, double y, double z) override;
	virtual void SetProjectionNormal(int normal);
	virtual void SetProjectionPosition(double x, double y, double z);

	virtual void EnableNavigation(bool flag);

	virtual void AddOne();
	virtual void FocalOne(int id);
	virtual void DeleteOne();
	virtual void DeleteOne(int id);
	virtual void DeleteAll();

	virtual void SaveWidgetToSeedData();
	virtual void SynRefresh();
	/**
	 * @brief	Update #SeedsData.
	 * Check whether oldPos exists in #SeedData, if exists, replace it. 
	 * OtherWise, insert newPos to #SeedData
	 * @param	oldPos the point world coordinate to check whether it exists
	 *			or not. If oldPos = newPos, it will insert point in most case.
	 * @param	newPos the point world coordinate to insert or update.
	 *			If newPos is nullptr, the point of oldPos will be removed. 
	 */
	virtual void UpdateSeedsData(const double* oldPos, const double* newPos);


protected:
	InteractorStyleSeedsPlacer();
	virtual ~InteractorStyleSeedsPlacer() override;


	virtual void uniqueInstall() override;
	//virtual void uniqueEnable() override;
	//virtual void uniqueDisable() override;

	virtual void GenerateWidgetFromSeedsData();
	virtual void ClearSeedsWidget();
	static QString ItemTranslate(vtkIdType id, const double pos[3]);

	double DisplayThickness;

	bool NavigationFlag;

	vtkSeedWidget* SeedsWidget;
	vtkBoundedPlanePointPlacer* PointPlacer;

	PolyData* SeedsData;
	Ui::InteractorStyleSeedsPlacer* ui;
};

	}
}

#endif // !__QVTK_INTERACTOR_STYLE_SEEDS_PLACER_H__
