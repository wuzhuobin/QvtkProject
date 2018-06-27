// me
#include "QvtkInteractorStyleDentalPlanning.h"
#include "ui_QvtkInteractorStyleDentalPlanning.h"
#include "QvtkScene.h"
#include "QvtkImplant.h"
#include "QvtkPolyDataActor.h"
#include "QvtkPolyDataActor2D.h"
#include "QvtkOrthogonalViewer.h"
#include "QvtkImage.h"
#include "QvtkVolume.h"

// vtk 
#include <vtkObjectFactory.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkPolyData.h>
#include <vtkPicker.h>
#include <vtkRenderWindow.h>

// qt
#include <QDebug>
namespace Q {
	namespace vtk {

vtkStandardNewMacro(InteractorStyleDentalPlanning);
Implant* InteractionImplant = nullptr;
Implant* HoveringImplant = nullptr;
Implant* SelectingImplant = nullptr;
void InteractorStyleDentalPlanning::setCustomEnable(bool flag)
{
	UniqueUiInteractorObserver::setCustomEnable(flag);
	Scene* scene = Scene::getCurrentScene();
	if (flag) {

		if (getViewer()->inherits("PlanarViewer")) {
			this->AnnotationRenderer = getViewer()->GetRenderers()[1];
		}
		else {
			this->AnnotationRenderer = this->DefaultRenderer;
		}

		if (!this->DefaultRenderer || !this->AnnotationRenderer) {
			vtkErrorMacro(<< "DefaultRenderer and AnnotationRenderer is a nullptr");
			return;
		}

		QStringList implants = scene->getAllDataByTag("Implant");
		foreach(QString implant, implants) {
			foreach(Prop* prop, *scene->getDataByUniqueName<Implant>(implant)->getReferenceProps()) {
				connect(prop, &Prop::picked,
					this, &InteractorStyleDentalPlanning::FindPickedImplant);
			}
		}

	}
	else {
		QStringList implants = scene->getAllDataByTag("Implant");
		foreach(QString implant, implants) {
			foreach(Prop* prop, *scene->getDataByUniqueName<Implant>(implant)->getReferenceProps()) {
				disconnect(prop, &Prop::picked,
					this, &InteractorStyleDentalPlanning::FindPickedImplant);
			}
		}
	}
}

void InteractorStyleDentalPlanning::install()
{
	UNIQUE_UI_INSTALL(InteractorStyleDentalPlanning);
	//connect(this->m_ui->checkBoxCursorFollowsImplant, &QCheckBox::stateChanged,
	//	this, [this](int state) {
	//	SetCursorFollowsImplant(state);
	//});
	connect(this->ui->checkBoxCursorFollowsImplant, &QCheckBox::stateChanged,
		this, &InteractorStyleDentalPlanning::SetCursorFollowsImplant);
}

void InteractorStyleDentalPlanning::uninstall()
{
	UNIQUE_UI_UNINSTALL();
}

//#include <vtkProperty.h>
void InteractorStyleDentalPlanning::AddImplant(unsigned int source)
{
	Scene* scene = Scene::getCurrentScene();
	
	const double* pos = getViewer()->getCursorPosition();

	Implant* implant = static_cast<Implant*>(scene->addNewDataByTag("Implant", "Wital Implant"));
	//implant->readData();
	implant->setOpacity(1);
	implant->setColor(1, 0, 0);
	implant->setSelectedColor(0, 1, 0);
	implant->setHoverColor(1, 0, 1);
	implant->setPickable(true);
	implant->setImplantSource(source);

	//vtkTransform* trans = vtkTransform::New();
	//trans->SetMatrix(implant->getUserMatrix());
	//trans->Translate()
	implant->getUserMatrix()->SetElement(0, 3, pos[0]);
	implant->getUserMatrix()->SetElement(1, 3, pos[1]);
	implant->getUserMatrix()->SetElement(2, 3, pos[2]);

	SYNCHRONAL_CALL(InteractorStyleDentalPlanning, 
	if (observer->getViewer()->inherits("PlanarViewer")) {
		PolyDataActor2D* polyDataActor2D = scene->createDataByClassName<PolyDataActor2D>();
		polyDataActor2D->setRenderDataSet(implant);
		polyDataActor2D->setPlanarOrientation(observer->getViewer()->GetOrientation());
		polyDataActor2D->setPlanarOrigin(pos[0], pos[1], pos[2]);
		scene->addData(polyDataActor2D, "WitalImplant2D");
		observer->getViewer()->AddProp(polyDataActor2D, observer->getViewer()->GetRenderers()[1]);
	}
	else
	{ 
		PolyDataActor* polyDataActor = scene->createDataByClassName<PolyDataActor>();
		polyDataActor->setRenderDataSet(implant);
		scene->addData(polyDataActor, "WitalImplant3D");
		observer->getViewer()->AddProp(polyDataActor, observer->getViewer()->GetRenderers()[0]);

	}

	observer->Interactor->Render();

	foreach(Prop* prop, *implant->getReferenceProps()) {
		connect(prop, &Prop::picked,
			observer, &InteractorStyleDentalPlanning::FindPickedImplant);
	}

	);



	//const QList<InteractorObserver*> dentalPlannings =
	//	m_stylesDataBase.values("InteractorStyleDentalPlanning");

	//foreach(InteractorObserver* style, dentalPlannings) {
	//	InteractorStyleDentalPlanning* dentalPlanning = static_cast<InteractorStyleDentalPlanning*>(style);
	//	
	//	PolyDataActor* actor = static_cast<PolyDataActor*>(scene->addNewDataByTag("PolyDataActor"));
	//	actor->setRenderDataSet(implant);
	//	actor->getActor()->GetProperty()->SetOpacity(0.2);
	//	dentalPlanning->getViewer()->AddProp(actor);

	//	PolyDataActor2D* polyDataActor2D = static_cast<PolyDataActor2D*>(scene->addNewDataByTag("PolyDataActor2D"));
	//	polyDataActor2D->setRenderDataSet(implant);
	//	polyDataActor2D->setPlanarOrientation(dentalPlanning->getViewer()->GetOrientation());
	//	polyDataActor2D->getActor()->GetProperty()->SetColor(1, 0, 0);
	//	dentalPlanning->getViewer()->AddProp(polyDataActor2D, 
	//		dentalPlanning->getViewer()->GetRenderers()[1]);
	//	getViewer()->ResetCamera(0);
	//	getViewer()->ResetCameraClippingRange(0);
	//	dentalPlanning->Interactor->Render();
	//}

}

void InteractorStyleDentalPlanning::OnMouseMove()
{
	vtkInteractorStyleTrackballActor::OnMouseMove();
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	switch (this->State)
	{
	case VTKIS_NAVIGATION:
		this->FindPokedRenderer(x, y);
		this->Navigation();
		this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
		break;
	case VTKIS_DOLLY:
		SYNCHRONAL_CALL(InteractorStyleDentalPlanning,
			observer->DefaultRenderer->GetActiveCamera()->SetParallelScale(this->CurrentRenderer->GetActiveCamera()->GetParallelScale()););
		break;
	case VTKIS_NONE:
		this->CurrentRenderer = this->AnnotationRenderer;
		this->FindPickedActor(x, y);
		this->Hover();
		this->CurrentRenderer = this->DefaultRenderer;
		break;
	}
	if (this->State != VTKIS_NONE) {
		SYNCHRONAL_CALL(InteractorStyleDentalPlanning,
			observer->Interactor->Render());
	}
}

void InteractorStyleDentalPlanning::OnLeftButtonDown()
{
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	//this->FindPokedRenderer(x, y);
	this->CurrentRenderer = this->AnnotationRenderer;
	this->FindPickedActor(x, y);
	// doing select
	this->Select();
	SYNCHRONAL_CALL(InteractorStyleDentalPlanning,
		observer->Interactor->Render());
	// using default renderer to have correct clipping range. 

	//if (this->InteractionProp)
	//{
	//	this->CurrentRenderer = this->AnnotationRenderer;
	//}
	//else
	//{
		this->CurrentRenderer = this->DefaultRenderer;
	//}

	this->GrabFocus(this->EventCallbackCommand);

	if (this->Interactor->GetShiftKey())
	{
		if (this->Interactor->GetControlKey())
		{
			this->StartDolly();
		}
		else
		{
			this->StartPan();
		}
	}
	else if (this->Interactor->GetControlKey())
	{
		this->StartSpin();
	}
	else
	{

		if(this->getViewer()->inherits("PlanarViewer")){
			if (this->InteractionProp) {
				this->StartSpin();
			}
			else {
				this->StartNavigation();
			}
		}
		else {
			this->StartRotate();

		}
	}
}

void InteractorStyleDentalPlanning::OnLeftButtonUp()
{
	switch (this->State)
	{
	case VTKIS_DOLLY:
		this->EndDolly();
		break;

	case VTKIS_PAN:
		this->EndPan();
		break;

	case VTKIS_SPIN:
		this->EndSpin();
		break;

	case VTKIS_ROTATE:
		this->EndRotate();
		break;
		
	case VTKIS_NAVIGATION:
		this->EndNavigation();
		break;
	}

	if (this->Interactor)
	{
		this->ReleaseFocus();
	}
}


void InteractorStyleDentalPlanning::OnMiddleButtonDown()
{
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	this->CurrentRenderer = this->AnnotationRenderer;
	this->FindPickedActor(x, y);
	// doing select
	this->Select();
	SYNCHRONAL_CALL(InteractorStyleDentalPlanning,
		observer->Interactor->Render());
	this->GrabFocus(this->EventCallbackCommand);

	//if (this->InteractionProp)
	//{
	//	this->CurrentRenderer = this->AnnotationRenderer;
	//}
	//else
	//{
		this->CurrentRenderer = this->DefaultRenderer;
	//}

	if (this->Interactor->GetControlKey())
	{
		this->StartDolly();
	}
	else
	{
		this->StartPan();
	}

}

void InteractorStyleDentalPlanning::OnMiddleButtonUp()
{
	switch (this->State)
	{
	case VTKIS_DOLLY:
		this->EndDolly();
		break;

	case VTKIS_PAN:
		this->EndPan();
		break;
	}

	if (this->Interactor)
	{
		this->ReleaseFocus();
	}
}

void InteractorStyleDentalPlanning::OnRightButtonDown()
{

	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	
	this->CurrentRenderer = this->AnnotationRenderer;
	this->FindPickedActor(x, y);
	// doing select
	this->Select();
	SYNCHRONAL_CALL(InteractorStyleDentalPlanning,
		observer->Interactor->Render());
	this->GrabFocus(this->EventCallbackCommand);

	//if (this->InteractionProp)
	//{
	//	this->CurrentRenderer = this->AnnotationRenderer;
	//	// disable scaling.
	//	//this->StartUniformScale();
	//}
	//else
	//{
		this->CurrentRenderer = this->DefaultRenderer;
		this->StartDolly();
	//}

}

void InteractorStyleDentalPlanning::OnRightButtonUp()
{
	switch (this->State)
	{
	case VTKIS_USCALE:
		this->EndUniformScale();
		break;
	case VTKIS_DOLLY:
		this->EndDolly();
		break;
	}

	if (this->Interactor)
	{
		this->ReleaseFocus();
	}
}

void InteractorStyleDentalPlanning::Navigation()
{
	double index[3];
	if (CalculateIndex(index)) {
		SetCursorPosition(index);
	}
}

void InteractorStyleDentalPlanning::Rotate()
{
	if (this->InteractionProp) {
		vtkInteractorStyleTrackballActor::Rotate();
		if (this->CursorFollowsImplant) {
			double center[3] = {
				this->InteractionProp->GetUserMatrix()->GetElement(0, 3),
				this->InteractionProp->GetUserMatrix()->GetElement(1, 3),
				this->InteractionProp->GetUserMatrix()->GetElement(2, 3),
			};
			SetCursorPosition(center);
		}
	}
	else {
		vtkRenderWindowInteractor *rwi = this->Interactor;

		int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
		int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

		int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

		double delta_elevation = -20.0 / size[1];
		double delta_azimuth = -20.0 / size[0];

		double rxf = dx * delta_azimuth * this->MotionFactor;
		double ryf = dy * delta_elevation * this->MotionFactor;

		vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
		camera->Azimuth(rxf);
		camera->Elevation(ryf);
		camera->OrthogonalizeViewUp();

		if (this->AutoAdjustCameraClippingRange)
		{
			this->CurrentRenderer->ResetCameraClippingRange();
		}

		if (rwi->GetLightFollowCamera())
		{
			this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
		}

		rwi->Render();
	}
}

void InteractorStyleDentalPlanning::Spin()
{
	if (this->InteractionProp) {
		vtkInteractorStyleTrackballActor::Spin();
		if (this->CursorFollowsImplant) {
			double center[3] = {
				this->InteractionProp->GetUserMatrix()->GetElement(0, 3),
				this->InteractionProp->GetUserMatrix()->GetElement(1, 3),
				this->InteractionProp->GetUserMatrix()->GetElement(2, 3),
			};
			SetCursorPosition(center);
		}
	}
	else {
		vtkRenderWindowInteractor *rwi = this->Interactor;

		double *center = this->CurrentRenderer->GetCenter();

		double newAngle =
			vtkMath::DegreesFromRadians(atan2(rwi->GetEventPosition()[1] - center[1],
				rwi->GetEventPosition()[0] - center[0]));

		double oldAngle =
			vtkMath::DegreesFromRadians(atan2(rwi->GetLastEventPosition()[1] - center[1],
				rwi->GetLastEventPosition()[0] - center[0]));

		vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
		camera->Roll(newAngle - oldAngle);
		camera->OrthogonalizeViewUp();

		rwi->Render();
	}
}

void InteractorStyleDentalPlanning::Pan()
{
	if (this->InteractionProp) {
		vtkInteractorStyleTrackballActor::Pan();
		if (this->CursorFollowsImplant) {
			double center[3] = {
				this->InteractionProp->GetUserMatrix()->GetElement(0, 3),
				this->InteractionProp->GetUserMatrix()->GetElement(1, 3),
				this->InteractionProp->GetUserMatrix()->GetElement(2, 3),
			};
			SetCursorPosition(center);
		}
	}
	else {

		vtkRenderWindowInteractor *rwi = this->Interactor;

		double viewFocus[4], focalDepth, viewPoint[3];
		double newPickPoint[4], oldPickPoint[4], motionVector[3];

		// Calculate the focal depth since we'll be using it a lot

		vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
		camera->GetFocalPoint(viewFocus);
		this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2],
			viewFocus);
		focalDepth = viewFocus[2];

		this->ComputeDisplayToWorld(rwi->GetEventPosition()[0],
			rwi->GetEventPosition()[1],
			focalDepth,
			newPickPoint);

		// Has to recalc old mouse point since the viewport has moved,
		// so can't move it outside the loop

		this->ComputeDisplayToWorld(rwi->GetLastEventPosition()[0],
			rwi->GetLastEventPosition()[1],
			focalDepth,
			oldPickPoint);

		// Camera motion is reversed

		motionVector[0] = oldPickPoint[0] - newPickPoint[0];
		motionVector[1] = oldPickPoint[1] - newPickPoint[1];
		motionVector[2] = oldPickPoint[2] - newPickPoint[2];

		camera->GetFocalPoint(viewFocus);
		camera->GetPosition(viewPoint);
		camera->SetFocalPoint(motionVector[0] + viewFocus[0],
			motionVector[1] + viewFocus[1],
			motionVector[2] + viewFocus[2]);

		camera->SetPosition(motionVector[0] + viewPoint[0],
			motionVector[1] + viewPoint[1],
			motionVector[2] + viewPoint[2]);

		if (rwi->GetLightFollowCamera())
		{
			this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
		}

		rwi->Render();
	}
}

void InteractorStyleDentalPlanning::Dolly()
{
	if (this->InteractionProp)
	{
		vtkInteractorStyleTrackballActor::Dolly();
		if (this->CursorFollowsImplant) {
			double center[3] = {
				this->InteractionProp->GetUserMatrix()->GetElement(0, 3),
				this->InteractionProp->GetUserMatrix()->GetElement(1, 3),
				this->InteractionProp->GetUserMatrix()->GetElement(2, 3),
			};
			SetCursorPosition(center);
		}
	}
	else {
		vtkRenderWindowInteractor *rwi = this->Interactor;
		double *center = this->CurrentRenderer->GetCenter();
		int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
		double dyf = this->MotionFactor * dy / center[1];

		vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
		if (camera->GetParallelProjection())
		{
			camera->SetParallelScale(camera->GetParallelScale() / pow(1.1, dyf));
		}
		else
		{
			camera->Dolly(pow(1.1, dyf));
			if (this->AutoAdjustCameraClippingRange)
			{
				this->CurrentRenderer->ResetCameraClippingRange();
			}
		}

		if (this->Interactor->GetLightFollowCamera())
		{
			this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
		}

		this->Interactor->Render();

	}
}

void InteractorStyleDentalPlanning::UniformScale()
{
	vtkInteractorStyleTrackballActor::UniformScale();
}

void InteractorStyleDentalPlanning::StartNavigation()
{
	if (this->State != VTKIS_NONE) {
		return;
	}
	this->StartState(VTKIS_NAVIGATION);
	// Navigation for the first click.
	Navigation();
}

void InteractorStyleDentalPlanning::EndNavigation()
{
	if (this->State != VTKIS_NAVIGATION) {
		return;
	}
	// Navigation for the first click.
	Navigation();
	this->StopState();
}

InteractorStyleDentalPlanning::InteractorStyleDentalPlanning()
{
	this->MotionFactor = 10.0;
	this->CursorFollowsImplant = true;
	//this->InteractionImplant = nullptr;
	//this->HoveringImplant = nullptr;
	//this->SelectingImplant = nullptr;
}

InteractorStyleDentalPlanning::~InteractorStyleDentalPlanning()
{
}

void InteractorStyleDentalPlanning::uniqueInstall()
{
	connect(this->ui->pushButtonAddNail, &QPushButton::clicked,
		this, &InteractorStyleDentalPlanning::AddImplant);

	connect(this->ui->pushButtonDeleteNail, &QPushButton::clicked,
		this, &InteractorStyleDentalPlanning::DeleteImplant);
}

void InteractorStyleDentalPlanning::SetCursorPosition(double x, double y, double z)
{
	// for NonPlanarViewer.
	//SYNCHRONAL_CALL(InteractorStyleDentalPlanning, 
	//	if (style->getViewer()->inherits("PlanarViewer"))
	//	{
	//		style->getViewer()->SetCursorPosition(x, y, z);
	//	}
	//);
	this->getViewer()->SetCursorPosition(x, y, z);
}


bool InteractorStyleDentalPlanning::CalculateIndex(double index[3])
{
	//Pick
	this->GetInteractor()->GetPicker()->Pick(
		this->GetInteractor()->GetEventPosition()[0],
		this->GetInteractor()->GetEventPosition()[1],
		0,  // always zero.
		this->CurrentRenderer);

	double* picked = this->GetInteractor()->GetPicker()->GetPickPosition();

	//Check if valid pick
	if (picked[0] == 0.0&&picked[1] == 0.0 && picked[2] == 0.0)
		return false;

	index[0] = picked[0];
	index[1] = picked[1];
	index[2] = picked[2];

	return true;
}


void InteractorStyleDentalPlanning::uniqueEnable()
{
	//Scene* scene = Scene::getCurrentScene();
	//QStringList implants = scene->getAllDataByTag("Implant");

	//// only set pickable to implants
	//foreach(QString implant, implants)
	//{
	//	Implant* _implant = static_cast<Implant*>(scene->getDataByUniqueName(implant));
	//	_implant->setPickable(true);
	//}

	//SCBImage* image = static_cast<SCBImage*>(scene->getDataByUniqueName("CBCT"));
	//image->setPickable(true);
	///// TODO
	//// remove this hard code
	//QStringList volumes = scene->getAllDataByClassName("SCBVolume");
	//SCBVolume* volume = static_cast<SCBVolume*>(scene->getDataByUniqueName(volumes[0]));
	//volume->getProp()->SetPickable(false);

}

void InteractorStyleDentalPlanning::uniqueDisable()
{
	//Scene* scene = Scene::getCurrentScene();
	//QStringList implants = scene->getAllDataByTag("Implant");

	//// only set pickable to implants
	//foreach(QString implant, implants)
	//{
	//	Implant* _implant = static_cast<Implant*>(scene->getDataByUniqueName(implant));
	//	_implant->setPickable(false);
	//}

	//SCBImage* image = static_cast<SCBImage*>(scene->getDataByUniqueName("CBCT"));
	//image->setPickable(false);
}

void InteractorStyleDentalPlanning::Hover()
{
	// nothing changed in this situation.
	if (this->InteractionProp &&
		InteractionImplant == HoveringImplant)
	{
		return;
	}
	// nothing pikced
	if (!this->InteractionProp) {
		InteractionImplant = nullptr;

		//if (this->HoveringImplant) {
		//	this->HoveringImplant->setAnnotationStatusToNone();
		//	this->HoveringImplant = this->InteractionImplant;
		//}
		//
		//Scene* scene = Scene::getCurrentScene();
		//QStringList implants = scene->getAllDataByTag("Nail");

		//foreach(QString implant, implants)
		//{
		//	Implant* _implant = static_cast<Implant*>(scene->getDataByUniqueName(implant));
		//	_implant->setAnnotationStatusToNone();
		//}

		//this->InteractionImplant = nullptr;
		//this->HoveringImplant = nullptr;
		//return;
	}

	// hovering
	if (HoveringImplant) {
		if (HoveringImplant != SelectingImplant) {
			HoveringImplant->setAnnotationStatusToNone();
			SYNCHRONAL_CALL(InteractorStyleDentalPlanning,
				observer->Interactor->Render());
		}

	}
	
	HoveringImplant = InteractionImplant;

	if (HoveringImplant) {

		if (HoveringImplant != SelectingImplant) {
			HoveringImplant->setAnnotationStatusToHover();
			SYNCHRONAL_CALL(InteractorStyleDentalPlanning,
				observer->Interactor->Render());
		}
	}

	//Scene* scene = Scene::getCurrentScene();
	//QStringList implants = scene->getAllDataByTag("Nail");

	//foreach(QString implant, implants)
	//{
	//	Implant* _implant = static_cast<Implant*>(scene->getDataByUniqueName(implant));
	//	_implant->setAnnotationStatusToNone();
	//}

	//this->InteractionImplant->setAnnotationStatusToHover();
}

void InteractorStyleDentalPlanning::Select()
{
	if (this->InteractionProp && 
		InteractionImplant == SelectingImplant)
	{
		return;
	}
	
	if (!this->InteractionProp) {
		this->InteractionProp = nullptr;
	}

	if (InteractionImplant == SelectingImplant) {
		return;
	}

	if (SelectingImplant) {
		SelectingImplant->setAnnotationStatusToNone();
	}
	SelectingImplant = InteractionImplant;
	if (SelectingImplant) {
		SelectingImplant->setAnnotationStatusToSelected();
	}
}

void InteractorStyleDentalPlanning::FindPickedImplant()
{
	InteractionImplant = nullptr;
	Prop* porp = qobject_cast<Prop*>(sender());
	if (porp) {
		InteractionImplant = qobject_cast<Implant*>(porp->getRenderDataSet());
	}
}

void InteractorStyleDentalPlanning::DeleteImplant()
{
	if (!SelectingImplant) {
		return;
	}

	Scene* scene = Scene::getCurrentScene();
	foreach(Prop* prop, *SelectingImplant->getReferenceProps()) {

		SYNCHRONAL_CALL(InteractorStyleDentalPlanning,
			observer->getViewer()->RemoveProp(prop);
			scene->removeData(prop);
			observer->Interactor->Render();
		);
	}
	scene->removeData(SelectingImplant);


	if (InteractionImplant == SelectingImplant) {
		InteractionImplant = nullptr;
	}

	if (HoveringImplant == SelectingImplant) {
		HoveringImplant = nullptr;
	}
	SelectingImplant = nullptr;
}

void InteractorStyleDentalPlanning::uniqueFunction()
{
	InteractionImplant = nullptr;
	HoveringImplant = nullptr;
	SelectingImplant = nullptr;
}

	}
}