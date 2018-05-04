#ifndef __QVTK_WIDGET_COLLECTION_H__
#define __QVTK_WIDGET_COLLECTION_H__
#pragma once
// me
#include "vtkWidgetCollectionBase.h"
#include "vtkPlanarSeedWidget.h"
#include "QvtkPlanarSeedWidget.h"
#include "QvtkImplantWidget.h"
#include "QvtkPolyDataSourceWidget.h"
#include "QvtkBiopsyWidget.h"
// qt
#include <QList>
#define QVTK_DESTORY_WIDGETS(NAME, CLASS) \
void Destroy##NAME(CLASS * widget = nullptr) \
{ \
	if(widget == nullptr){ \
		if(this->NAME.size() == 0) { \
			return; \
		} \
		widget = this->NAME.back(); \
	} \
	widget->SetInteractor(nullptr); \
	widget->uninstall(); \
	this->NAME.removeOne(widget); \
} 
#define QVTK_PRODUCE_WIDGETS(NAME, CLASS) \
CLASS* Produce##NAME() \
{ \
	vtkSmartPointer<CLASS> widget = vtkSmartPointer<CLASS>::New(); \
	widget->install(); \
	this->NAME.push_back(widget); \
	widget->setViewer(this->Viewer); \
	widget->SetInteractor(this->Interactor); \
	widget->SetDefaultRenderer(this->DefaultRenderer); \
	return widget; \
}
#define QVTK_SET_ALL_WIDGETS_ENABLED(NAME, CLASS) \
void SetAll##NAME##Enabled(bool flag) \
{ \
	for (QList<vtkSmartPointer<CLASS>>::const_iterator cit = this->NAME.cbegin(); \
		cit != this->NAME.cend(); ++cit) \
	{ \
		if(flag) \
		{ \
			this->InternalUpdate(*cit, flag); \
		} \
		(*cit)->SetEnabled(flag); \
		if(!flag) \
		{ \
			this->InternalUpdate(*cit, flag); \
		} \
	} \
}
#define QVTK_SET_ONE_OF_WIDGETS_ENABLED(NAME, CLASS) \
void SetOneOf##NAME##Enabled(CLASS* widget, bool flag) \
{ \
	if(flag) \
	{ \
		this->InternalUpdate(widget, flag); \
	} \
	widget->SetEnabled(flag); \
	if(!flag) \
	{ \
		this->InternalUpdate(widget, flag); \
	} \
}
#define QVTK_GET_WIDGETS(NAME, CLASS) \
virtual QList<vtkSmartPointer<CLASS>> Get##NAME() {return this->NAME;}
#define QVTK_SETUP_WIDGETS(NAME, CLASS) \
QVTK_DESTORY_WIDGETS(NAME, CLASS) \
QVTK_PRODUCE_WIDGETS(NAME, CLASS) \
QVTK_SET_ALL_WIDGETS_ENABLED(NAME, CLASS) \
QVTK_GET_WIDGETS(NAME, CLASS) \
QVTK_SET_ONE_OF_WIDGETS_ENABLED(NAME, CLASS)
namespace Q {
	namespace vtk {
		class /*WIDGET_EXPORT*/ WidgetCollection : public vtkWidgetCollectionBase
		{
		public:
			static WidgetCollection* New() { return new WidgetCollection; }
			vtkTypeMacro(WidgetCollection, vtkWidgetCollectionBase);
			virtual void SetViewer(OrthogonalViewer* viewer) { this->Viewer = viewer; }
			SETUP_WIDGETS(VTKPlanarSeedWidgets, vtkPlanarSeedWidget);
			QVTK_SETUP_WIDGETS(PlanarSeedWidgets, PlanarSeedWidget);
			QVTK_SETUP_WIDGETS(ImplantWidgets, ImplantWidget);
			QVTK_SETUP_WIDGETS(PolyDataSourceWidgets, PolyDataSourceWidget);
			QVTK_SETUP_WIDGETS(BiopsyWidgets, BiopsyWidget);
		protected:
			WidgetCollection() {}
			virtual ~WidgetCollection() override {}
			virtual void InternalUpdate(vtkInteractorObserver* widget, bool flag) override
			{
				InteractorObserver* _widget =
					dynamic_cast<InteractorObserver*>(widget);
				if (_widget) {
					_widget->setCustomEnable(flag);
				}
			}
			std::list<vtkSmartPointer<vtkPlanarSeedWidget>> VTKPlanarSeedWidgets;
			QList<vtkSmartPointer<PlanarSeedWidget>> PlanarSeedWidgets;
			QList<vtkSmartPointer<ImplantWidget>> ImplantWidgets;
			QList<vtkSmartPointer<PolyDataSourceWidget>> PolyDataSourceWidgets;
			QList<vtkSmartPointer<BiopsyWidget>> BiopsyWidgets;
			OrthogonalViewer* Viewer;
		private:
			WidgetCollection(const WidgetCollection&) VTK_DELETE_FUNCTION;
			void operator=(const WidgetCollection&) VTK_DELETE_FUNCTION;
		};
	}
}



#endif // !__QVTK_WIDGET_COLLECTION_H__



