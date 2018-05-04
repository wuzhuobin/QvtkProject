/*
Author:		Wong, Matthew Lun
Date:		16th, June 2016
Occupation:	Chinese University of Hong Kong,
Department of Imaging and Inteventional Radiology,
Junior Research Assistant
This class is based on vtkvtkInteractorStyleGeneralSwitchBase, written to allow easy
switching between 2D interactors.
Wong Matthew Lun
Copyright (C) 2016
*/


#ifndef __QVTK_STYLES_SWITCH_ORTHOGONAL_VIEWER_H__
#define __QVTK_STYLES_SWITCH_ORTHOGONAL_VIEWER_H__

// me
#include "vtkInteractorStyleGeneralSwitchBase.h"
#include "QvtkInteractorStyleWindowLevel.h"
#include "QvtkInteractorStyleNavigation.h"
#include "QvtkInteractorStyleDentalPlanning.h"
#include "QvtkInteractorStyleSeedsPlacer.h"

#define NEW_SCB_INTERACTOR_STYLE(NAME, CLASS) \
NEW_INTERACTOR_STYLE(NAME, CLASS) \
allOrthogonalViewrerStyles.push_back(NAME);
namespace Q {
	namespace vtk {
		class OrthogonalViewer;
		class StylesSwitchOrthogonalViewer :
			public vtkInteractorStyleGeneralSwitchBase,
			public InteractorStyleOrthogonalViewer

		{
		public:
			static StylesSwitchOrthogonalViewer* New() { return new StylesSwitchOrthogonalViewer; }
			vtkTypeMacro(StylesSwitchOrthogonalViewer, vtkInteractorStyleGeneralSwitchBase);
			SETUP_INTERACTOR_STYLE(WindowLevel, InteractorStyleWindowLevel);
			SETUP_INTERACTOR_STYLE(Navigation, InteractorStyleNavigation);
			SETUP_INTERACTOR_STYLE(DentalPlanning, InteractorStyleDentalPlanning);
			SETUP_INTERACTOR_STYLE(SeedPlacer, InteractorStyleSeedsPlacer);

			virtual void SetViewer(OrthogonalViewer* viewer) {
				InteractorStyleOrthogonalViewer::setViewer(viewer);
				foreach(UniqueUiInteractorObserver* _style, allOrthogonalViewrerStyles) {
					InteractorStyleOrthogonalViewer* __style = dynamic_cast<InteractorStyleOrthogonalViewer*>(_style);
					if (__style) {
						__style->setViewer(viewer);
					}
				}
			}


		protected:
			StylesSwitchOrthogonalViewer() {
				// The first instance of QInteractorStyleNavigation should have control 
				// of the ui
				// begin inserting your styles
				NEW_SCB_INTERACTOR_STYLE(WindowLevel, InteractorStyleWindowLevel);
				NEW_SCB_INTERACTOR_STYLE(Navigation, InteractorStyleNavigation);
				NEW_SCB_INTERACTOR_STYLE(DentalPlanning, InteractorStyleDentalPlanning);
				NEW_SCB_INTERACTOR_STYLE(SeedPlacer, InteractorStyleSeedsPlacer);
				// end inserting your styles

				foreach(UniqueUiInteractorObserver* _style, allOrthogonalViewrerStyles) {
					_style->install();
				}

			}
			virtual ~StylesSwitchOrthogonalViewer() override {
				foreach(UniqueUiInteractorObserver* _style, allOrthogonalViewrerStyles) {
					_style->uninstall();
				}
			}


			virtual void InternalUpdate(bool flag) override {
				// using enable update to call #EnabledOn() and #EnabledOff()
				// #TODO dynamic_cast are not a good option
				// try not to use RTTI of c++ 
				UniqueUiInteractorObserver* _style =
					dynamic_cast<UniqueUiInteractorObserver*>(this->CurrentStyle);
				if (_style) {
					_style->setCustomEnable(flag);
				}
			}

			InteractorStyleWindowLevel* WindowLevel;
			InteractorStyleNavigation* Navigation;
			InteractorStyleDentalPlanning* DentalPlanning;
			InteractorStyleSeedsPlacer* SeedPlacer;

			QList<UniqueUiInteractorObserver*> allOrthogonalViewrerStyles;

		};


	}
}

#endif // !__QVTK_STYLES_SWITCH_ORTHOGONAL_VIEWER_H__