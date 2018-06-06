#ifndef __QVTK_INTERACTOR_STYLE_DENTAL_PLANNING_H__
#define __QVTK_INTERACTOR_STYLE_DENTAL_PLANNING_H__


// me
#include "QvtkInteractorStyleOrthogonalViewer.h"
#include "QvtkUniqueUiInteractorObserver.h"
namespace Q {
	namespace vtk {
		namespace Ui { class InteractorStyleDentalPlanning; }
		class Implant;
	}
}
// vtk 
#include <vtkInteractorStyleTrackballActor.h>
class vtkRenderer;

// qt
#include <QWidget>

namespace Q {
	namespace vtk {
class InteractorStyleDentalPlanning : 
	public UniqueUiInteractorObserver,
	public InteractorStyleOrthogonalViewer, 
	public vtkInteractorStyleTrackballActor
{
	Q_OBJECT;
public:
	static const int VTKIS_NAVIGATION = 1024;
	vtkTypeMacro(InteractorStyleDentalPlanning, vtkInteractorStyleTrackballActor);
	static InteractorStyleDentalPlanning* New();
	
	Ui::InteractorStyleDentalPlanning* getUi() { return this->ui; }
	virtual void setCustomEnable(bool flag) override;
	virtual void install() override;
	virtual void uninstall() override;

	virtual void OnMouseMove() override;

	virtual void OnLeftButtonDown() override;
	virtual void OnLeftButtonUp() override;
	virtual void OnMiddleButtonDown() override;
	virtual void OnMiddleButtonUp() override;
	virtual void OnRightButtonDown() override;
	virtual void OnRightButtonUp() override;

	virtual void Navigation();
	virtual void Rotate() override;
	virtual void Spin() override;
	virtual void Pan() override;
	virtual void Dolly() override;
	virtual void UniformScale() override;
	virtual void Hover();
	virtual void Select();


	virtual void StartNavigation();
	virtual void EndNavigation();

	virtual double GetMotionFactor() { return this->MotionFactor; }
	virtual bool GetCursorFollowsImPlant() { return this->CursorFollowsImplant; }

public slots:

	virtual void SetMotionFactor(double factor) { this->MotionFactor = factor; }

	virtual void SetCursorFollowsImplant(bool flag) { this->CursorFollowsImplant = flag; }

	virtual void AddImplant(unsigned int source);

	virtual void DeleteImplant();

	virtual void FindPickedImplant();

protected:
	InteractorStyleDentalPlanning();
	virtual ~InteractorStyleDentalPlanning() override;


	virtual void uniqueInstall() override;
	virtual void uniqueUninstall() override {}
	virtual void uniqueFunction() override;
	virtual void uniqueEnable() override;
	virtual void uniqueDisable() override;

	virtual void SetCursorPosition(const double index[3]) { SetCursorPosition(index[0], index[1], index[2]); }
	virtual void SetCursorPosition(double x, double y, double z);
	//virtual void CentralCursorPosition(double index[3]);
	virtual bool CalculateIndex(double index[3]);

	//SCBImage* Image;
	bool CursorFollowsImplant;

	double MotionFactor;

	vtkRenderer* AnnotationRenderer;

	Ui::InteractorStyleDentalPlanning* ui;



};


	}
}
#endif // !__QVTK_INTERACTOR_STYLE_DENTAL_PLANNING_H__
