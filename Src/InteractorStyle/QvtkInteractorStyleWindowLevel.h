#ifndef __SCB_INTERACTOR_STYLE_WINDOW_LEVEL_H__
#define __SCB_INTERACTOR_STYLE_WINDOW_LEVEL_H__
#pragma once
// me
#include "interactorstyle_export.h"
#include "QvtkInteractorStyleOrthogonalViewer.h"
#include "vtkInteractorStyleWindowLevel.h"
#include "QvtkUniqueUiInteractorObserver.h"
namespace Q {
	namespace vtk {
		namespace Ui { class InteractorStyleWindowLevel; }
		class Image;
	}
}
namespace Q {
	namespace vtk {
		class INTERACTORSTYLE_EXPORT InteractorStyleWindowLevel : public UniqueUiInteractorObserver,
			public InteractorStyleOrthogonalViewer,
			public vtkInteractorStyleWindowLevel
		{
			Q_OBJECT;
		public:
			vtkTypeMacro(InteractorStyleWindowLevel, vtkInteractorStyleWindowLevel);
			static InteractorStyleWindowLevel* New();

			Ui::InteractorStyleWindowLevel* getUi() { return this->ui; }
			virtual void setCustomEnable(bool flag) override;
			virtual void install() override;
			virtual void uninstall() override;

			virtual void OnLeftButtonDown() override;
			virtual void OnLeftButtonUp() override;

			public slots:
			virtual void SetWindow(int window);
			virtual void SetWindow(double window) override;
			virtual double GetWindow() override;
			virtual void SetLevel(int level);
			virtual void SetLevel(double level) override;
			virtual double GetLevel() override;

			virtual double GetResetWindow() override;
			virtual double GetResetLevel() override;

			virtual void SynchronalRender();

		protected:
			InteractorStyleWindowLevel();
			virtual ~InteractorStyleWindowLevel() override;
			virtual void uniqueInstall() override;
			virtual void uniqueUninstall() override {}
			virtual void uniqueFunction() override {}
			virtual void uniqueEnable() override {}
			virtual void uniqueDisable() override {}
			virtual int tryPick(const int XY[2] = nullptr) override;
			virtual DataSet* findPokedDataSet(const int XY[2] = nullptr) override;
			virtual Image* FindPokedImage();
			Ui::InteractorStyleWindowLevel* ui;
			static Image* pokedImage;

		};
	}
}
#endif // !__SCB_INTERACTOR_STYLE_WINDOW_LEVEL_H__
