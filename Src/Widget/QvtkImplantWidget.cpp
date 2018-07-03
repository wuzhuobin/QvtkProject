// me
#include "QvtkImplantWidget.h"
#include "QvtkScene.h"
#include "QvtkOrthogonalViewer.h"
#include "QvtkPolyDataActor.h"
#include "QvtkImplant.h"
#include "vtkAffineRepresentation2D2.h"


// vtk
#include <vtkRenderer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkSmartPointer.h>
#include <vtkCallbackCommand.h>
#include <vtkTransform.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>
#include <vtkCamera.h>
namespace Q {
	namespace vtk {

class ImplantWidgetCameraCallback : public vtkCommand 
{
public:
	static ImplantWidgetCameraCallback* New() { return new ImplantWidgetCameraCallback; }
	vtkTypeMacro(ImplantWidgetCameraCallback, vtkCommand);

	virtual void Execute(vtkObject *caller, unsigned long eventId,
		void *callData) VTK_OVERRIDE 
	{
		// Please set the this->Self.
		if (!this->Self) {
			return;
		}
		if (this->Self->GetDefaultRenderer()->GetActiveCamera() == caller) {
			// Must call for Update the Representation;
			this->Self->GetRepresentation()->Modified();
		}
		this->Self->Render();
	}
	ImplantWidget* Self = nullptr;

};


vtkStandardNewMacro(ImplantWidget);

//vtkCxxSetObjectMacro(ImplantWidget, Matrix, vtkMatrix4x4);

void ImplantWidget::PrintSelf(ostream & os, vtkIndent indent)
{
}

void ImplantWidget::setCustomEnable(bool flag)
{
	NoUiInteractorObserver::setCustomEnable(flag);
	Scene* scene = Scene::getCurrentScene();
	if (flag)
	{
		//if (!this->Implant)
		//{
		//	vtkErrorMacro(<< "Implant is a nullptr. Please set implant to render. ");
		//	return;
		//}
		//this->ImplantActor->setRenderDataSet(this->Implant);
		this->ImplantWidgetProp->getProp()->GetUserMatrix()->SetElement(0, 3, this->getViewer()->getCursorPosition()[0]);
		this->ImplantWidgetProp->getProp()->GetUserMatrix()->SetElement(1, 3, this->getViewer()->getCursorPosition()[1]);
		this->ImplantWidgetProp->getProp()->GetUserMatrix()->SetElement(2, 3, this->getViewer()->getCursorPosition()[2]);
		static_cast<vtkAffineRepresentation2D2*>(this->GetRepresentation())->SetUserMatrix(
			this->ImplantWidgetProp->getProp()->GetUserMatrix());
		static_cast<vtkAffineRepresentation2D2*>(this->GetRepresentation())->SetOrientation(
			this->getViewer()->getOrientation());
		connect(this->getViewer(), &OrthogonalViewer::OrientationChanged,
			this, [&](int orientation) {
			static_cast<vtkAffineRepresentation2D2*>(this->GetRepresentation())->SetOrientation(orientation);
		});


		this->getViewer()->addProp(this->ImplantWidgetProp);
		//this->GetAffineRepresentation()->PlaceWidget();

		//static_cast<vtkAffineRepresentation2D*>(
		//	this->GetAffineRepresentation())->SetOrigin(
		//		this->Implant->getUserMatrix()->GetElement(0, 3),
		//		this->Implant->getUserMatrix()->GetElement(1, 3),
		//		this->Implant->getUserMatrix()->GetElement(2, 3)
		//	);

	}
	else
	{
		this->getViewer()->removeProp(this->ImplantWidgetProp);
	}
}

void ImplantWidget::SetEnabled(int i)
{
	Superclass::SetEnabled(i);
	vtkCamera* camera = this->DefaultRenderer->GetActiveCamera();
	if (i) {
		//this->ImplantWidgetProp->getProp()->GetUserMatrix()->AddObserver(vtkCommand::ModifiedEvent, this->CameraCallback);
		camera->AddObserver(vtkCommand::ModifiedEvent, this->CameraCallback);
	}
	else {
		camera->RemoveObserver(this->CameraCallback);
		//this->ImplantWidgetProp->getProp()->GetUserMatrix()->RemoveObserver(this->CameraCallback);
	}
}

ImplantWidget::ImplantWidget()
{
	//this->CreateDefaultRepresentation();
	this->SetWidgetRepresentation(vtkSmartPointer<vtkAffineRepresentation2D2>::New());

	//this->Matrix = nullptr;

	this->ImplantWidgetProp = nullptr;
	this->ImplantWidgetProp = new PolyDataActor;
	//this->Implant = nullptr;

	this->CameraCallback = ImplantWidgetCameraCallback::New();
	this->CameraCallback->Self = this;

	//this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
	//	vtkWidgetEvent::EndSelect,
	//	this, ImplantWidget::EndSelectAction);
	//this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
	//	vtkWidgetEvent::Move,
	//	this, ImplantWidget::MoveAction);
	//this->CallbackMapper->SetCallbackMethod(vtkCommand::LeaveEvent, )
}
//
//void ImplantWidget::EndSelectAction(vtkAbstractWidget * w)
//{
//	ImplantWidget* self = static_cast<ImplantWidget*>(w);
//	if (self->WidgetState != vtkAffineWidget::Active)
//	{
//		return;
//	}
//
//	int X = self->Interactor->GetEventPosition()[0];
//	int Y = self->Interactor->GetEventPosition()[1];
//	double eventPos[2];
//	eventPos[0] = static_cast<double>(X);
//	eventPos[1] = static_cast<double>(Y);
//	self->WidgetRep->EndWidgetInteraction(eventPos);
//
//	// return to initial state
//	self->WidgetState = vtkAffineWidget::Start;
//	self->ModifierActive = 0;
//
//	// Highlight as necessary
//	self->WidgetRep->Highlight(0);
//
//	// stop adjusting
//	self->EventCallbackCommand->SetAbortFlag(1);
//	self->ReleaseFocus();
//	self->EndInteraction();
//
//	// update the implant's user matrix
//	if (self->Prop) {
//		vtkSmartPointer<vtkTransform> transform =
//			vtkSmartPointer<vtkTransform>::New();
//		vtkAffineRepresentation* affineRep = static_cast<vtkAffineRepresentation*>(self->WidgetRep);
//		affineRep->GetTransform(transform);
//		//transform->PostMultiply();
//		//transform->Concatenate(self->Implant->getUserMatrix());
//		self->Prop->getProp()->GetUserMatrix()->DeepCopy(transform->GetMatrix());
//	}
//
//	self->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
//	self->WidgetState = vtkAffineWidget::Start;
//	self->Render();
//}
//
//void ImplantWidget::MoveAction(vtkAbstractWidget * w)
//{
//	ImplantWidget* self = static_cast<ImplantWidget*>(w);
//	// compute some info we need for all cases
//	int X = self->Interactor->GetEventPosition()[0];
//	int Y = self->Interactor->GetEventPosition()[1];
//
//	// Set the cursor appropriately
//	if (self->WidgetState == vtkAffineWidget::Start)
//	{
//		self->ModifierActive = self->Interactor->GetShiftKey() |
//			self->Interactor->GetControlKey();
//		int state = self->WidgetRep->GetInteractionState();
//		reinterpret_cast<vtkAffineRepresentation*>(self->WidgetRep)->
//			ComputeInteractionState(X, Y, self->ModifierActive);
//		self->SetCursor(self->WidgetRep->GetInteractionState());
//		if (state != self->WidgetRep->GetInteractionState())
//		{
//			self->Render();
//		}
//		return;
//	}
//
//	// Okay, adjust the representation
//	double eventPosition[2];
//	eventPosition[0] = static_cast<double>(X);
//	eventPosition[1] = static_cast<double>(Y);
//	self->WidgetRep->WidgetInteraction(eventPosition);
//
//	// update the implant's user matrix
//	if (self->Prop) {
//		vtkSmartPointer<vtkTransform> transform =
//			vtkSmartPointer<vtkTransform>::New();
//		vtkAffineRepresentation* affineRep = static_cast<vtkAffineRepresentation*>(self->WidgetRep);
//		affineRep->GetTransform(transform);
//		//transform->PostMultiply();
//		//transform->Concatenate(self->Implant->getUserMatrix());
//		self->Prop->getProp()->GetUserMatrix()->DeepCopy(transform->GetMatrix());
//	}
//
//
//
//	// Got this event, we are finished
//	self->EventCallbackCommand->SetAbortFlag(1);
//	self->InvokeEvent(vtkCommand::InteractionEvent, NULL);
//	self->Render();
//}

ImplantWidget::~ImplantWidget()
{
	this->CameraCallback->Delete();

	//this->Implant = nullptr;
	delete this->ImplantWidgetProp;
	//this->ImplantWidgetProp = nullptr;
}

	}
}
