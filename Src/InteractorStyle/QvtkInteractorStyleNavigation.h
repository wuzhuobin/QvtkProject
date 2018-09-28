#ifndef __QVTK_INTERACTOR_STYLE_NAVIGATION_H__
#define __QVTK_INTERACTOR_STYLE_NAVIGATION_H__
#pragma once
// me 
#include "interactorstyle_export.h"
#include "QvtkInteractorStyleOrthogonalViewer.h"
#include "vtkInteractorStyleNavigation.h"
#include "QvtkUniqueUiInteractorObserver.h"
namespace Q {
	namespace vtk {
	namespace Ui { class InteractorStyleNavigation; }
	class Image;
	class Volume;
	}
}
// vtk
class vtkVolumePicker;
class vtkPropPicker;
namespace Q {
	namespace vtk {
		class INTERACTORSTYLE_EXPORT InteractorStyleNavigation :
			public UniqueUiInteractorObserver,
			public InteractorStyleOrthogonalViewer,
			public vtkInteractorStyleNavigation
		{
			Q_OBJECT;
		public:
			static InteractorStyleNavigation *New();
			vtkTypeMacro(InteractorStyleNavigation, vtkInteractorStyleNavigation);
			void PrintSelf(ostream& os, vtkIndent indent);

			virtual void setCustomEnable(bool flag) override;
			virtual void install() override;
			virtual void uninstall() override;
			Ui::InteractorStyleNavigation* getUi() { return this->ui; }
			virtual bool GetFast();
			virtual void OnLeftButtonDown() override;
			virtual void OnLeftButtonUp() override;

			public Q_SLOTS:

			virtual void SetCursorPosition(double x, double y, double z) override;
			virtual void SetCursorPosition(const double xyz[3]) override { this->SetCursorPosition(xyz[0], xyz[1], xyz[2]); }
			virtual void SetCursorPosition(const int ijk[3]) { this->SetCursorPosition(ijk[0], ijk[1], ijk[2]); }
			virtual void SetCursorPosition(int i, int j, int k);
			virtual void CentralizeCursorPosition() override;
			virtual void SetCameraScale(double scale) override;

			virtual void SetFast(bool fast);
			virtual void SetCursorPositionX(double x);
			virtual void SetCursorPositionY(double y);
			virtual void SetCursorPositionZ(double z);
			virtual void SetCursorPositionI(int i);
			virtual void SetCursorPositionJ(int j);
			virtual void SetCursorPositionK(int k);

		protected:

			InteractorStyleNavigation();
			virtual ~InteractorStyleNavigation() override;

			virtual bool CalculateIndex(double index[3]);

			virtual void uniqueInstall() override;
			virtual void uniqueUninstall() override {}
			virtual void uniqueFunction() override {}
			virtual void uniqueEnable() override {}
			virtual void uniqueDisable() override {}
			virtual int tryPick(const int XY[2] = nullptr) override;
			vtkVolumePicker* VolumePicker;
			mutable double pickedPos[3];
			Ui::InteractorStyleNavigation* ui;
		};
	}
}


#endif // !__QVTK_INTERACTOR_STYLE_NAVIGATION_H__
