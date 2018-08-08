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

			virtual void OnLeftButtonDown() override;
			virtual void OnLeftButtonUp() override;

			public Q_SLOTS:

			virtual void SetCursorPosition(double x, double y, double z) override;
			virtual void CentralizeCursorPosition() override;
			virtual void SetCameraScale(double scale) override;

			virtual void SetCursorPositionX(double x);
			virtual void SetCursorPositionY(double y);
			virtual void SetCursorPositionZ(double z);
			virtual void SetCursorPositionI(double i);
			virtual void SetCursorPositionJ(double j);
			virtual void SetCursorPositionK(double k);

		protected:

			InteractorStyleNavigation();
			virtual ~InteractorStyleNavigation() override;

			virtual bool CalculateIndex(double index[3]);

			virtual void uniqueInstall() override;
			virtual void uniqueUninstall() override {}
			virtual void uniqueFunction() override {}
			virtual void uniqueEnable() override {}
			virtual void uniqueDisable() override {}
			virtual int tryPick(double xyz[3] = nullptr) override;
			vtkVolumePicker* VolumePicker;

			Ui::InteractorStyleNavigation* ui;
		};
	}
}


#endif // !__QVTK_INTERACTOR_STYLE_NAVIGATION_H__
