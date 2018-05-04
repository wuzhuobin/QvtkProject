#include "DepthIndicatorWidget.h"
#include "DepthIndicatorRepresentation.h"
#include "vtkCallbackCommand.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(DepthIndicatorWidget);

//-------------------------------------------------------------------------
DepthIndicatorWidget::DepthIndicatorWidget()
{
	this->Selectable = 1;
	this->Resizable = 0;
	//this->PickingManagedOff();
}

//-------------------------------------------------------------------------
DepthIndicatorWidget::~DepthIndicatorWidget()
{
}

void DepthIndicatorWidget::SetRepresentation(DepthIndicatorRepresentation * r)
{
	this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));
}

//----------------------------------------------------------------------
void DepthIndicatorWidget::CreateDefaultRepresentation()
{
	if (!this->WidgetRep)
	{
		this->WidgetRep = DepthIndicatorRepresentation::New();
	}
}

//-------------------------------------------------------------------------
void DepthIndicatorWidget::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}