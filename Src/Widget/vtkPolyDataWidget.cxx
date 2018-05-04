// me
#include "vtkPolyDataWidget.h"
// vtk
#include <vtkObjectFactory.h>
#include <vtkProp3D.h>
#include <vtkMatrix4x4.h>
#include <vtkPropPicker.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkTransform.h>
// std
#include <algorithm>
vtkCxxSetObjectMacro(vtkPolyDataWidget, PropProperty, vtkProperty);
vtkStandardNewMacro(vtkPolyDataWidget);
void vtkPolyDataWidget::PrintSelf(ostream & os, vtkIndent indent)
{
}

void vtkPolyDataWidget::SetProp3D(vtkProp3D * prop)
{
	Superclass::SetProp3D(prop);
	if (prop) {
		this->PropPicker->AddPickList(this->Prop3D);
	}
	this->PropPicker->PickFromListOn();
}

void vtkPolyDataWidget::PlaceWidget(double x, double y, double z)
{
	if (!this->Prop3D) {
		vtkErrorMacro(<< "this->Prop3D is a nullptr. " << "Please SetProp3D() first.");
		return;
	}
	this->Prop3D->GetUserMatrix()->SetElement(0, 3, x);
	this->Prop3D->GetUserMatrix()->SetElement(1, 3, y);
	this->Prop3D->GetUserMatrix()->SetElement(2, 3, z);
	this->Render();
}

void vtkPolyDataWidget::UpdatePlacement()
{
		vtkMatrix4x4 *oldMatrix = vtkMatrix4x4::New();
		this->Prop3D->GetMatrix(oldMatrix);

		double orig[3];
		this->Prop3D->GetOrigin(orig);

		vtkTransform *newTransform = vtkTransform::New();
		newTransform->PostMultiply();
		if (this->Prop3D->GetUserMatrix() != NULL)
		{
			newTransform->SetMatrix(this->Prop3D->GetUserMatrix());
		}
		else
		{
			newTransform->SetMatrix(oldMatrix);
		}

		newTransform->Translate(-(this->BoxCenter[0]), -(this->BoxCenter[1]), -(this->BoxCenter[2]));

		for (int i = 0; i <this->NumRotation; i++)
		{
			newTransform->RotateWXYZ(this->Rotate2x4[i][0], this->Rotate2x4[i][1],
				this->Rotate2x4[i][2], this->Rotate2x4[i][3]);
		}

		if ((this->Scale[0] * this->Scale[1] * this->Scale[2]) != 0.0)
		{
			newTransform->Scale(this->Scale[0], this->Scale[1], this->Scale[2]);
		}

		newTransform->Translate(this->BoxCenter[0], this->BoxCenter[1], this->BoxCenter[2]);

		// now try to get the composit of translate, rotate, and scale
		newTransform->Translate(-(orig[0]), -(orig[1]), -(orig[2]));
		newTransform->PreMultiply();
		newTransform->Translate(orig[0], orig[1], orig[2]);

		if (this->Prop3D->GetUserMatrix() != NULL)
		{
			newTransform->GetMatrix(this->Prop3D->GetUserMatrix());
		}
		else
		{
			this->Prop3D->SetPosition(newTransform->GetPosition());
			this->Prop3D->SetScale(newTransform->GetScale());
			this->Prop3D->SetOrientation(newTransform->GetOrientation());
		}
		oldMatrix->Delete();
		newTransform->Delete();
}

void vtkPolyDataWidget::SetEnabled(int enabling)
{
	if (!this->Interactor)
	{
		//unsigned int State;
		vtkErrorMacro(<< "The interactor must be set prior to enabling/disabling widget.");
		return;
	}

	if (enabling) //-----------------------------------------------------------
	{
		vtkDebugMacro(<< "Enabling vtkPolyDataWidget.");

		if (this->Enabled) //already enabled, just return
		{
			return;
		}

		if (!this->CurrentRenderer)
		{
			this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(
				this->Interactor->GetLastEventPosition()[0],
				this->Interactor->GetLastEventPosition()[1]));
			if (this->CurrentRenderer == NULL)
			{
				return;
			}
		}

		this->Enabled = 1;

		// listen for the following events
		vtkRenderWindowInteractor *i = this->Interactor;
		i->AddObserver(vtkCommand::MouseMoveEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::LeftButtonPressEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::LeftButtonReleaseEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::MiddleButtonPressEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::MiddleButtonReleaseEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::RightButtonPressEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::RightButtonReleaseEvent,
			this->EventCallbackCommand, this->Priority);


		this->InvokeEvent(vtkCommand::EnableEvent, NULL);
		this->BuildRepresentation();
		this->Prop3D->PickableOn();
	}
	else //disabling----------------------------------------------------------
	{
		vtkDebugMacro(<< "Disabling PolyDataSourceWidget.");

		if (!this->Enabled) //already disabled, just return
		{
			return;
		}

		this->Enabled = 0;

		// don't listen for events any more
		this->Interactor->RemoveObserver(this->EventCallbackCommand);

		this->InvokeEvent(vtkCommand::DisableEvent, NULL);
		this->SetCurrentRenderer(NULL);

		this->BuildRepresentation();
		this->Prop3D->PickableOff();
	}

	if (this->CurrentRenderer) {
		this->CurrentRenderer->Render();
	}
}

void vtkPolyDataWidget::Render()
{
	if (this->CurrentRenderer) {
		this->CurrentRenderer->Render();
	}
}

vtkPolyDataWidget::vtkPolyDataWidget()
{
	this->PropPicker = vtkPropPicker::New();
	this->State = vtkPolyDataWidget::Start;
	this->EventCallbackCommand->SetCallback(vtkPolyDataWidget::ProcessEvents);
	this->EventCallbackCommand->SetClientData(this);

	this->PropProperty = nullptr;

	this->HoverProperty = vtkProperty::New();
	this->HoverProperty->SetColor(0, 1, 0);

	this->SelectedProperty = vtkProperty::New();
	this->SelectedProperty->SetColor(0, 0, 1);

	this->MotionFactor = 10;

}

vtkPolyDataWidget::~vtkPolyDataWidget()
{
	this->PropPicker->Delete();
	this->SetPropProperty(nullptr);
	this->HoverProperty->Delete();
	this->SelectedProperty->Delete();
}

void vtkPolyDataWidget::ProcessEvents(vtkObject * object, unsigned long event, void * clientdata, void * calldata)
{
	vtk3DWidget::ProcessEvents(object, event, clientdata, calldata);
	vtkPolyDataWidget* self = static_cast<vtkPolyDataWidget*>(clientdata);

	//okay, let's do the right thing
	switch (event)
	{
	case vtkCommand::LeftButtonPressEvent:
		self->OnLeftButtonDown();
		break;
	case vtkCommand::LeftButtonReleaseEvent:
		self->OnLeftButtonUp();
		break;
	case vtkCommand::MiddleButtonPressEvent:
		self->OnMiddleButtonDown();
		break;
	case vtkCommand::MiddleButtonReleaseEvent:
		self->OnMiddleButtonUp();
		break;
	case vtkCommand::RightButtonPressEvent:
		self->OnRightButtonDown();
		break;
	case vtkCommand::RightButtonReleaseEvent:
		self->OnRightButtonUp();
		break;
	case vtkCommand::MouseMoveEvent:
		self->OnMouseMove();
		break;
	}
}

bool vtkPolyDataWidget::FoundPickedProp(int x, int y)
{	
	if (!this->Prop3D)
	{
		vtkErrorMacro(<< "this->Prop3D is a nullptr. " << "Please SetProp3D(). ");
		return false;
	}
	if (!this->PropPicker->Pick(x, y, 0, this->CurrentRenderer)) {
		return false;
	}
	if (this->PropPicker->GetViewProp() != this->Prop3D) {
		return false;
	}
	return true;
}

void vtkPolyDataWidget::BuildRepresentation()
{
	if (this->Enabled) {
		this->CurrentRenderer->AddViewProp(this->Prop3D);
	}
	else {
		this->CurrentRenderer->RemoveViewProp(this->Prop3D);
	}
}

void vtkPolyDataWidget::StartAction(unsigned int state)
{
	if (this->State != vtkPolyDataWidget::Start){
		return;
	}
	this->State = state;
	this->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
}

void vtkPolyDataWidget::EndAction(unsigned int state)
{
	if (this->State != state) {
		return;
	}
	this->State = vtkPolyDataWidget::Start;
	this->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
	this->Render();
}

void vtkPolyDataWidget::Rotate()
{
	if (this->CurrentRenderer == NULL || this->Prop3D == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;
	vtkCamera *cam = this->CurrentRenderer->GetActiveCamera();

	// First get the origin of the assembly
	//double *obj_center = this->Prop3D->GetCenter();
	std::copy_n(this->Prop3D->GetCenter(), 3, this->BoxCenter);

	// GetLength gets the length of the diagonal of the bounding box
	double boundRadius = this->Prop3D->GetLength() * 0.5;

	// Get the view up and view right vectors
	double view_up[3], view_look[3], view_right[3];

	cam->OrthogonalizeViewUp();
	cam->ComputeViewPlaneNormal();
	cam->GetViewUp(view_up);
	vtkMath::Normalize(view_up);
	cam->GetViewPlaneNormal(view_look);
	vtkMath::Cross(view_up, view_look, view_right);
	vtkMath::Normalize(view_right);

	// Get the furtherest point from object position+origin
	double outsidept[3];

	outsidept[0] = this->BoxCenter[0] + view_right[0] * boundRadius;
	outsidept[1] = this->BoxCenter[1] + view_right[1] * boundRadius;
	outsidept[2] = this->BoxCenter[2] + view_right[2] * boundRadius;

	// Convert them to display coord
	double disp_obj_center[3];

	this->ComputeWorldToDisplay(this->BoxCenter[0], this->BoxCenter[1], this->BoxCenter[2],
		disp_obj_center);

	this->ComputeWorldToDisplay(outsidept[0], outsidept[1], outsidept[2],
		outsidept);

	double radius = sqrt(vtkMath::Distance2BetweenPoints(disp_obj_center,
		outsidept));
	double nxf = (rwi->GetEventPosition()[0] - disp_obj_center[0]) / radius;

	double nyf = (rwi->GetEventPosition()[1] - disp_obj_center[1]) / radius;

	double oxf = (rwi->GetLastEventPosition()[0] - disp_obj_center[0]) / radius;

	double oyf = (rwi->GetLastEventPosition()[1] - disp_obj_center[1]) / radius;

	if (((nxf * nxf + nyf * nyf) <= 1.0) &&
		((oxf * oxf + oyf * oyf) <= 1.0))
	{
		double newXAngle = vtkMath::DegreesFromRadians(asin(nxf));
		double newYAngle = vtkMath::DegreesFromRadians(asin(nyf));
		double oldXAngle = vtkMath::DegreesFromRadians(asin(oxf));
		double oldYAngle = vtkMath::DegreesFromRadians(asin(oyf));

		this->Scale[0] = this->Scale[1] = this->Scale[2] = 1.0;

		this->Rotate2x4[0][0] = newXAngle - oldXAngle;
		this->Rotate2x4[0][1] = view_up[0];
		this->Rotate2x4[0][2] = view_up[1];
		this->Rotate2x4[0][3] = view_up[2];

		this->Rotate2x4[1][0] = oldYAngle - newYAngle;
		this->Rotate2x4[1][1] = view_right[0];
		this->Rotate2x4[1][2] = view_right[1];
		this->Rotate2x4[1][3] = view_right[2];

		this->NumRotation = 2;
		this->UpdatePlacement();
		this->CurrentRenderer->ResetCameraClippingRange();

		this->Render();
	}
}

void vtkPolyDataWidget::Spin()
{
		if (this->CurrentRenderer == NULL || this->Prop3D == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;
	vtkCamera *cam = this->CurrentRenderer->GetActiveCamera();

	// Get the axis to rotate around = vector from eye to origin
	std::copy_n(this->Prop3D->GetCenter(), 3, this->BoxCenter);


	double motion_vector[3];
	double view_point[3];

	if (cam->GetParallelProjection())
	{
		// If parallel projection, want to get the view plane normal...
		cam->ComputeViewPlaneNormal();
		cam->GetViewPlaneNormal(motion_vector);
	}
	else
	{
		// Perspective projection, get vector from eye to center of actor
		cam->GetPosition(view_point);
		motion_vector[0] = view_point[0] - this->BoxCenter[0];
		motion_vector[1] = view_point[1] - this->BoxCenter[1];
		motion_vector[2] = view_point[2] - this->BoxCenter[2];
		vtkMath::Normalize(motion_vector);
	}

	double disp_obj_center[3];

	this->ComputeWorldToDisplay(this->BoxCenter[0], this->BoxCenter[1], this->BoxCenter[2],
		disp_obj_center);

	double newAngle =
		vtkMath::DegreesFromRadians(atan2(rwi->GetEventPosition()[1] - disp_obj_center[1],
			rwi->GetEventPosition()[0] - disp_obj_center[0]));

	double oldAngle =
		vtkMath::DegreesFromRadians(atan2(rwi->GetLastEventPosition()[1] - disp_obj_center[1],
			rwi->GetLastEventPosition()[0] - disp_obj_center[0]));

	this->Scale[0] = this->Scale[1] = this->Scale[2] = 1.0;



	this->Rotate2x4[0][0] = newAngle - oldAngle;
	this->Rotate2x4[0][1] = motion_vector[0];
	this->Rotate2x4[0][2] = motion_vector[1];
	this->Rotate2x4[0][3] = motion_vector[2];

	this->NumRotation = 1;
	this->UpdatePlacement();



	this->CurrentRenderer->ResetCameraClippingRange();
	this->Render();
}

void vtkPolyDataWidget::Pan()
{
	  if (this->CurrentRenderer == NULL || this->Prop3D == NULL)
  {
    return;
  }

  vtkRenderWindowInteractor *rwi = this->Interactor;

  // Use initial center as the origin from which to pan

  double *obj_center = this->Prop3D->GetCenter();

  double disp_obj_center[3], new_pick_point[4];
  double old_pick_point[4], motion_vector[3];

  this->ComputeWorldToDisplay(obj_center[0], obj_center[1], obj_center[2],
                              disp_obj_center);

  this->ComputeDisplayToWorld(rwi->GetEventPosition()[0],
                              rwi->GetEventPosition()[1],
                              disp_obj_center[2],
                              new_pick_point);

  this->ComputeDisplayToWorld(rwi->GetLastEventPosition()[0],
                              rwi->GetLastEventPosition()[1],
                              disp_obj_center[2],
                              old_pick_point);

  motion_vector[0] = new_pick_point[0] - old_pick_point[0];
  motion_vector[1] = new_pick_point[1] - old_pick_point[1];
  motion_vector[2] = new_pick_point[2] - old_pick_point[2];

  if (this->Prop3D->GetUserMatrix() != NULL)
  {
    vtkTransform *t = vtkTransform::New();
    t->PostMultiply();
    t->SetMatrix(this->Prop3D->GetUserMatrix());
    t->Translate(motion_vector[0], motion_vector[1], motion_vector[2]);
    this->Prop3D->GetUserMatrix()->DeepCopy(t->GetMatrix());
    t->Delete();
  }
  else
  {
    this->Prop3D->AddPosition(motion_vector[0],
                                       motion_vector[1],
                                       motion_vector[2]);
  }


    this->CurrentRenderer->ResetCameraClippingRange();

	this->Render();
}

void vtkPolyDataWidget::Dolly()
{
	if (this->CurrentRenderer == NULL || this->Prop3D == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;
	vtkCamera *cam = this->CurrentRenderer->GetActiveCamera();

	double view_point[3], view_focus[3];
	double motion_vector[3];

	cam->GetPosition(view_point);
	cam->GetFocalPoint(view_focus);

	double *center = this->CurrentRenderer->GetCenter();

	int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
	double yf = dy / center[1] * this->MotionFactor;
	double dollyFactor = pow(1.1, yf);

	dollyFactor -= 1.0;
	motion_vector[0] = (view_point[0] - view_focus[0]) * dollyFactor;
	motion_vector[1] = (view_point[1] - view_focus[1]) * dollyFactor;
	motion_vector[2] = (view_point[2] - view_focus[2]) * dollyFactor;

	if (this->Prop3D->GetUserMatrix() != NULL)
	{
		vtkTransform *t = vtkTransform::New();
		t->PostMultiply();
		t->SetMatrix(this->Prop3D->GetUserMatrix());
		t->Translate(motion_vector[0], motion_vector[1],
			motion_vector[2]);
		this->Prop3D->GetUserMatrix()->DeepCopy(t->GetMatrix());
		t->Delete();
	}
	else
	{
		this->Prop3D->AddPosition(motion_vector);
	}

	this->CurrentRenderer->ResetCameraClippingRange();

	this->Render();
}

void vtkPolyDataWidget::UniformScale()
{
	if (this->CurrentRenderer == NULL || this->Prop3D == NULL)
	{
		return;
	}

  vtkRenderWindowInteractor *rwi = this->Interactor;

  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

  std::copy_n(this->Prop3D->GetCenter(), 3, this->BoxCenter);
  double *center = this->CurrentRenderer->GetCenter();

  double yf = dy / center[1] * this->MotionFactor;
  double scaleFactor = pow(1.1, yf);

  this->Scale[0] = this->Scale[1] = this->Scale[2] = scaleFactor;

  this->NumRotation = 0;

  this->UpdatePlacement();

  this->CurrentRenderer->ResetCameraClippingRange();

  this->Render();
}

void vtkPolyDataWidget::Select(bool flag)
{
	vtkActor* actor = vtkActor::SafeDownCast(this->Prop3D);
	if (!actor) {
		return;
	}
	if (flag && (actor->GetProperty() == this->SelectedProperty)) {
		return;
	}

	if (!flag && (actor->GetProperty() == this->PropProperty)) {
		return;
	}

	if (actor->GetProperty() != this->SelectedProperty &&
		actor->GetProperty() != this->HoverProperty) {
		this->SetPropProperty(actor->GetProperty());
	}

	if (flag) {
		actor->SetProperty(this->SelectedProperty);
	}
	else {
		actor->SetProperty(this->PropProperty);
	}
}

void vtkPolyDataWidget::Hover(bool flag)
{
	vtkActor* actor = vtkActor::SafeDownCast(this->Prop3D);
	if (!actor) {
		return;
	}
	// seleceted, do not change.
	if (actor->GetProperty() == this->SelectedProperty) {
		return;
	}

	if (flag && (actor->GetProperty() == this->HoverProperty)) {
		return;
	}

	if (!flag && (actor->GetProperty() == this->PropProperty)) {
		return;
	}

	if (actor->GetProperty() != this->SelectedProperty &&
		actor->GetProperty() != this->HoverProperty) {

		this->SetPropProperty(actor->GetProperty());
	}
	if (flag) {
		actor->SetProperty(this->HoverProperty);
		this->Render();
	}
	else {
		actor->SetProperty(this->PropProperty);
		this->Render();
	}
}

void vtkPolyDataWidget::OnLeftButtonDown()
{
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];
	
	this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(x, y));
	if (!this->CurrentRenderer) {
		return;
	}

	if(!this->FoundPickedProp(x, y)){
		this->Select(false);
		return;
	}
	this->Select(true);
	this->EventCallbackCommand->AbortFlagOn();
	this->GrabFocus(this->EventCallbackCommand);

	if (this->Interactor->GetShiftKey())
	{
		if (this->Interactor->GetControlKey())
		{
			this->StartAction(vtkPolyDataWidget::Dollying);
		}
		else
		{
			this->StartAction(vtkPolyDataWidget::Panning);
		}
	}
	else if (this->Interactor->GetControlKey())
	{
		this->StartAction(vtkPolyDataWidget::Spinning);
	}
	else
	{
		this->StartAction(vtkPolyDataWidget::Rotating);
	}
}

void vtkPolyDataWidget::OnLeftButtonUp()
{
	this->EndAction(this->State);
	this->ReleaseFocus();
}

void vtkPolyDataWidget::OnMiddleButtonDown()
{
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(x, y));
	if (!this->CurrentRenderer) {
		return;
	}
	if (!this->FoundPickedProp(x, y)) {
		this->Select(false);
		return;
	}
	this->Select(true);
	this->EventCallbackCommand->AbortFlagOn();
	// doing select
	//this->Select();
	this->GrabFocus(this->EventCallbackCommand);

	if (this->Interactor->GetControlKey())
	{
		this->StartAction(vtkPolyDataWidget::Dollying);
	}
	else
	{
		this->StartAction(vtkPolyDataWidget::Panning);
	}
}

void vtkPolyDataWidget::OnMiddleButtonUp()
{
	this->EndAction(this->State);
	this->ReleaseFocus();
}

void vtkPolyDataWidget::OnRightButtonDown()
{
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(x, y));
	if (!this->CurrentRenderer) {
		return;
	}
	if (!this->FoundPickedProp(x, y)) {
		this->Select(false);
		return;
	}
	this->Select(true);
	this->EventCallbackCommand->AbortFlagOn();
	// doing select
	//this->Select();
	this->GrabFocus(this->EventCallbackCommand);

	this->StartAction(vtkPolyDataWidget::UniformScaling);
}

void vtkPolyDataWidget::OnRightButtonUp()
{	
	this->EndAction(this->State);
	this->ReleaseFocus();
}

void vtkPolyDataWidget::OnMouseMove()
{
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(x, y));
	if (!this->CurrentRenderer) {
		return;
	}

	switch (this->State)
	{
	case vtkPolyDataWidget::Start:
		if (this->Prop3D) {
			if (this->FoundPickedProp(x, y)) {
				this->Hover(true);
			}
			else {
				this->Hover(false);
			}
		}
		break;
	case vtkPolyDataWidget::Rotating:
		this->Rotate();
		break;
	case vtkPolyDataWidget::Spinning:
		this->Spin();
		break;
	case vtkPolyDataWidget::Panning:
		this->Pan();
		break;
	case vtkPolyDataWidget::Dollying:
		this->Dolly();
		break;
	case vtkPolyDataWidget::UniformScaling:
		this->UniformScale();
		break;
	default:
		break;
	}
}
