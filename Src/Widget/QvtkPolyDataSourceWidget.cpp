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
		if (!this->PolyData) {
			vtkErrorMacro(<< "this->PolyData is a nullptr. " << "Please SetPolyData(). ");
			return;
		}
		PlanarViewer* planarViewer = qobject_cast<PlanarViewer*>(this->getViewer());
		if (this->Prop) {
			delete this->Prop;
		}
		if (planarViewer) {
			PolyDataActor2D* actor2d = new PolyDataActor2D;
			this->getViewer()->AddProp(actor2d, this->getViewer()->GetRenderers()[1]);
			actor2d->setPlanarOrientation(planarViewer->GetOrientation());
			actor2d->setPlanarOrigin(
				planarViewer->GetCursorPosition()[0],
				planarViewer->GetCursorPosition()[1],
				planarViewer->GetCursorPosition()[2]);
			this->Prop = actor2d;
		}
		else {
			PolyDataActor* actor = new PolyDataActor;
			this->getViewer()->AddProp(actor);
			this->Prop = actor;
			//this->getViewer()->AddProp(this->Prop);
		}
		this->Prop->setRenderDataSet(this->PolyData);

		this->SetInputData(this->Prop->getRenderDataSet()->getDataSet());
		this->SetProp3D(this->Prop->getProp());

	}
	else {
		//this->annotationPolyData = nullptr;
		this->SetProp3D(nullptr);
		this->SetInputData(nullptr);

		this->Prop->setRenderDataSet(nullptr);
		this->getViewer()->RemoveProp(this->Prop);
		delete this->Prop;
		this->Prop = nullptr;
		this->PolyData = nullptr;
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
	emit static_cast<PolyDataSourceWidget*>(getUniqueThis("PolyDataSourceWidget"))->DisabledFollowCursor(m_unfollowCursorFlag);
}

void PolyDataSourceWidget::Render()
{
	synchronalCall(this, [&](InteractorObserver* ob) {
		PolyDataSourceWidget* widget = qobject_cast<PolyDataSourceWidget*>(ob);
		if (widget && widget->getViewer()) {
			widget->getViewer()->Render();
		}
	});
}

PolyDataSourceWidget::PolyDataSourceWidget()
{
	this->Prop = nullptr;
}

PolyDataSourceWidget::~PolyDataSourceWidget()
{
	this->Prop = nullptr;
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

	AnnotationPolyData* annotationPolyData = qobject_cast<AnnotationPolyData*>(this->PolyData);
	if (annotationPolyData) {
		if (flag && 
			annotationPolyData->getAnnotationStatus() != AnnotationPolyData::SELECTED) {
			SYNCHRONAL_CALL(
				PolyDataSourceWidget,
				observer->Select(false);
			);
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
	AnnotationPolyData* annotationPolyData = qobject_cast<AnnotationPolyData*>(this->PolyData);
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
		this->getViewer()->SetCursorPosition(
			this->Prop3D->GetUserMatrix()->GetElement(0, 3),
			this->Prop3D->GetUserMatrix()->GetElement(1, 3),
			this->Prop3D->GetUserMatrix()->GetElement(2, 3));
	}
}

	}
}
