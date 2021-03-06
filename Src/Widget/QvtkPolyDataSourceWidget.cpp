// me 
#include "ui_QvtkPolyDataSourceWidget.h"
#include "QvtkPolyDataSourceWidget.h"
#include "QvtkDataSet.h"
#include "QvtkPolyData.h"
#include "QvtkAnnotationPolyData.h"
#include "QvtkProp.h"
#include "QvtkScene.h"
#include "QvtkPlanarViewer.h"
#include "QvtkPolyDataActor.h"
#include "QvtkPolyDataActor2D.h"

// vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCallbackCommand.h>
#include <vtkPropPicker.h>
#include <vtkProp3D.h>
#include <vtkObjectFactory.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkCamera.h>

// qt
#include <QDebug>
namespace Q {
	namespace vtk {
vtkStandardNewMacro(PolyDataSourceWidget);
bool PolyDataSourceWidget::m_unfollowCursorFlag = false;
void PolyDataSourceWidget::PrintSelf(ostream & os, vtkIndent indent)
{
}

void PolyDataSourceWidget::setCustomEnable(bool flag)
{
	// actually PolyDataSource does need the customEnable and uniqueEnable things.
	// and it will lead to crash if it does not handle well. 
	//UniqueUiInteractorObserver::setCustomEnable(flag);
	InteractorObserver::setCustomEnable(flag);
	if (flag) {
		if (!this->SourceWidgetPolyData) {
			vtkErrorMacro(<< "this->SourceWidgetPolyData is a nullptr. " << "Please SetPolyData(). ");
			return;
		}
		PlanarViewer* planarViewer = qobject_cast<PlanarViewer*>(this->getViewer());
		if (this->SourceWidgetProp) {
			delete this->SourceWidgetProp;
		}
		if (planarViewer) {
			PolyDataActor2D* actor2d = new PolyDataActor2D;
			this->getViewer()->addProp(actor2d, this->getViewer()->getRenderers()[1]);
			actor2d->setPlanarNormal(this->getViewer()->getCurrentPlaneNormal());
			actor2d->setPlanarOrigin(
				planarViewer->getCursorPosition()[0],
				planarViewer->getCursorPosition()[1],
				planarViewer->getCursorPosition()[2]);
			this->SourceWidgetProp = actor2d;
		}
		else {
			PolyDataActor* actor = new PolyDataActor;
			this->getViewer()->addProp(actor);
			this->SourceWidgetProp = actor;
			//this->getViewer()->addProp(this->SourceWidgetProp);
		}
		this->SourceWidgetProp->setRenderDataSet(this->SourceWidgetPolyData);

		this->SetInputData(this->SourceWidgetProp->getRenderDataSet()->getDataSet());
		this->SetProp3D(this->SourceWidgetProp->getProp());

	}
	else {
		//this->annotationPolyData = nullptr;
		this->SetProp3D(nullptr);
		this->SetInputData(nullptr);

		this->SourceWidgetProp->setRenderDataSet(nullptr);
		this->getViewer()->removeProp(this->SourceWidgetProp);
		delete this->SourceWidgetProp;
		this->SourceWidgetProp = nullptr;
		this->SourceWidgetPolyData = nullptr;
	}
	this->Render();
}

void PolyDataSourceWidget::install()
{
	UNIQUE_UI_INSTALL(PolyDataSourceWidget);
}

void PolyDataSourceWidget::uninstall()
{
	UNIQUE_UI_UNINSTALL();
}

void PolyDataSourceWidget::DisableFollowCursor(bool flag)
{
	if (m_unfollowCursorFlag == flag) {
		return;
	}
	m_unfollowCursorFlag = flag;
	PolyDataSourceWidget *uniqueThis = qobject_cast<PolyDataSourceWidget*>(getUniqueThis("Q::vtk::PolyDataSourceWidget"));
	if (uniqueThis) {
		emit uniqueThis->DisabledFollowCursor(flag);
	}
}

void PolyDataSourceWidget::Render()
{
	FunctionSet2 f = [&](InteractorObserver* ob) {
		PolyDataSourceWidget* widget = qobject_cast<PolyDataSourceWidget*>(ob);
		if (widget && widget->getViewer()) {
			widget->getViewer()->update();
		}
	};
	synchronalCall(this, f);
}

PolyDataSourceWidget::PolyDataSourceWidget()
{
	this->SourceWidgetProp = nullptr;
}

PolyDataSourceWidget::~PolyDataSourceWidget()
{
	this->SourceWidgetProp = nullptr;
}

void PolyDataSourceWidget::uniqueInstall()
{
	connect(this->ui->checkBoxFollowCursor, &QCheckBox::toggled, &PolyDataSourceWidget::DisableFollowCursor);
	connect(this, &PolyDataSourceWidget::DisabledFollowCursor, this->ui->checkBoxFollowCursor, &QCheckBox::setChecked);
}

void PolyDataSourceWidget::Rotate()
{
	vtkPolyDataWidget::Rotate();
	this->FollowCursor();
}

void PolyDataSourceWidget::Spin()
{
	vtkPolyDataWidget::Spin();
	this->FollowCursor();
}

void PolyDataSourceWidget::Pan()
{
	vtkPolyDataWidget::Pan();
	this->FollowCursor();
}

void PolyDataSourceWidget::Dolly()
{
	vtkPolyDataWidget::Dolly();
	this->FollowCursor();
}

void PolyDataSourceWidget::UniformScale()
{
	vtkPolyDataWidget::UniformScale();
	this->FollowCursor();
}

void PolyDataSourceWidget::Select(bool flag)
{

	AnnotationPolyData* annotationPolyData = qobject_cast<AnnotationPolyData*>(this->SourceWidgetPolyData);
	if (annotationPolyData) {
		if (flag && 
			annotationPolyData->getAnnotationStatus() != AnnotationPolyData::SELECTED) {
			FunctionSet2 f = [](InteractorObserver *observer) {
				static_cast<PolyDataSourceWidget*>(observer)->Select(false); };
			synchronalCall(this, f);
			annotationPolyData->setAnnotationStatusToSelected();
		}
		else if(!flag && 
			annotationPolyData->getAnnotationStatus() != AnnotationPolyData::NONE){
			annotationPolyData->setAnnotationStatusToNone();
		}
	}
	else {
		vtkPolyDataWidget::Select(flag);
	}
}

void PolyDataSourceWidget::Hover(bool flag)
{
	AnnotationPolyData* annotationPolyData = qobject_cast<AnnotationPolyData*>(this->SourceWidgetPolyData);
	if (annotationPolyData) {
		if (annotationPolyData->getAnnotationStatus() == AnnotationPolyData::SELECTED) {
			return;
		}
		if (flag &&
			annotationPolyData->getAnnotationStatus() != AnnotationPolyData::HOVER) {
			annotationPolyData->setAnnotationStatusToHover();
		}
		else if (!flag &&
			annotationPolyData->getAnnotationStatus() != AnnotationPolyData::NONE) {
			annotationPolyData->setAnnotationStatusToNone();
		}
		this->Render();
	}
	else {
		vtkPolyDataWidget::Hover(flag);
	}
}

void PolyDataSourceWidget::FollowCursor()
{
	if (!m_unfollowCursorFlag) {
		this->getViewer()->setCursorPosition(
			this->Prop3D->GetUserMatrix()->GetElement(0, 3),
			this->Prop3D->GetUserMatrix()->GetElement(1, 3),
			this->Prop3D->GetUserMatrix()->GetElement(2, 3));
	}
}
	}
}
