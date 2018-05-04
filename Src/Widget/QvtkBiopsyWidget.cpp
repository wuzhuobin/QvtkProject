// me
#include "QvtkBiopsyWidget.h"
#include "QvtkProp.h"
#include "QvtkImageSlice.h"
#include "QvtkOrthogonalViewer.h"
#include "QvtkBiopsyData.h"
#include "vtkWidgetSet2.h"
#include "ui_QvtkBiopsyWidget.h"

// vtk
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkPointHandleRepresentation3D.h>
#include <vtkBoundedPlanePointPlacer.h>
#include <vtkFocalPlanePointPlacer.h>
#include <vtkWidgetSet.h>
#include <vtkLineRepresentation.h>
#include <vtkCallbackCommand.h>
#include <vtkWidgetEvent.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkTubeFilter.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPointPlacer.h>
#include <vtkPointHandleRepresentation3D.h>
#include <vtkFieldData.h>
#include <vtkDoubleArray.h>
#include <vtkLineWidget2.h>
#include <vtkLineRepresentation.h>
// qt
#include <QDebug>

class BiopsyWidgetRepresentation : public vtkLineRepresentation
{
public:
	static BiopsyWidgetRepresentation* New();
	vtkTypeMacro(BiopsyWidgetRepresentation, vtkLineRepresentation);
	virtual void PrintSelf(ostream& os, vtkIndent indent) override;
	vtkGetObjectMacro(TubeFilter, vtkTubeFilter);
protected:
	BiopsyWidgetRepresentation();
	virtual ~BiopsyWidgetRepresentation() override;
	vtkTubeFilter* TubeFilter;
};

namespace Q {
	namespace vtk {
vtkStandardNewMacro(BiopsyWidget);
const QString BiopsyWidget::RADIUS("RADIUS");
void BiopsyWidget::PrintSelf(ostream & os, vtkIndent indent)
{
	vtkLineWidget2::PrintSelf(os, indent);
}

void BiopsyWidget::setCustomEnable(bool flag)
{
	UniqueUiInteractorObserver::setCustomEnable(flag);
	if (flag) {
		QList<Prop*>& props = this->getViewer()->GetProps();
		
		this->SetProjectionNormal(this->getViewer()->GetOrientation());
		double* pos = this->getViewer()->GetCursorPosition();
		this->SetProjectionPosition(pos[0], pos[1], pos[2]);
		
		QObject::connect(this->getViewer(), &OrthogonalViewer::OrientationChanged,
			this, &BiopsyWidget::SetProjectionNormal);
		QObject::connect(this->getViewer(), &OrthogonalViewer::CursorPositionChanged,
			this, &BiopsyWidget::SetProjectionPosition);

		//this->biopsyData
		
		if (!this->m_biopsyData) {
			qCritical() << "BiopsyData is a nullptr. " << "The BiopsyWidget cannot access the data. ";
			return;
		}
		double point1[3];
		double point2[3];
		memcpy(point1, pos, sizeof(point1));
		memcpy(point2, pos, sizeof(point2));
		if (this->m_biopsyData->getPolyData()->GetPoints() && 
			this->m_biopsyData->getPolyData()->GetPoints()->GetNumberOfPoints() > 1) {
			this->m_biopsyData->getPolyData()->GetPoint(0, point1);
			this->m_biopsyData->getPolyData()->GetPoint(1, point2);
		}
		this->GetLineRepresentation()->SetPoint1WorldPosition(point1);
		this->GetLineRepresentation()->SetPoint2WorldPosition(point2);

		this->SetRadius(this->m_biopsyData->getRadius());
		QObject::connect(this->ui->doubleSpinBoxRadius, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
			this->m_biopsyData, &BiopsyData::setRadius);
	}
	else {
		QObject::disconnect(this->getViewer(), &OrthogonalViewer::OrientationChanged,
			this, &BiopsyWidget::SetProjectionNormal);
		QObject::disconnect(this->getViewer(), &OrthogonalViewer::CursorPositionChanged,
			this, &BiopsyWidget::SetProjectionPosition);
		if (this->m_biopsyData) {
			QObject::connect(this->ui->doubleSpinBoxRadius, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
				this->m_biopsyData, &BiopsyData::setRadius);
		}
	}
}

void BiopsyWidget::install()
{
	UNIQUE_UI_INSTALL(BiopsyWidget);

}

void BiopsyWidget::uninstall()
{
	UNIQUE_UI_UNINSTALL();
}

void BiopsyWidget::SetProjectionNormal(int normal)
{
	this->m_pointPlacer->SetProjectionNormal(normal);
}

void BiopsyWidget::SetProjectionPosition(double x, double y, double z)
{
	synchronalCall(this, [x, y, z](InteractorObserver* ib) {
		BiopsyWidget* widget = static_cast<BiopsyWidget*>(ib);

		switch (widget->m_pointPlacer->GetProjectionNormal())
		{
		case vtkBoundedPlanePointPlacer::XAxis:
			widget->m_pointPlacer->SetProjectionPosition(x);
			break;
		case vtkBoundedPlanePointPlacer::YAxis:
			widget->m_pointPlacer->SetProjectionPosition(y);
			break;
		case vtkBoundedPlanePointPlacer::ZAxis:
			widget->m_pointPlacer->SetProjectionPosition(z);
			break;
		default:
			break;
		}

	});
}

void BiopsyWidget::SetRadius(double radius)
{
	synchronalCall(this, [&radius](InteractorObserver* observer) {
		BiopsyWidget* widget = static_cast<BiopsyWidget*>(observer);
		reinterpret_cast<BiopsyWidgetRepresentation*>(widget->GetRepresentation())->GetTubeFilter()->SetRadius(radius);

		widget->Render();
	});
	this->ui->doubleSpinBoxRadius->setValue(radius);
}

void BiopsyWidget::CreateDefaultRepresentation()
{
	if (!this->WidgetRep) {
		
		BiopsyWidgetRepresentation* rep = BiopsyWidgetRepresentation::New();
		this->SetWidgetRepresentation(rep);
		rep->Delete();
		//vtkLineWidget2::CreateDefaultRepresentation();

		this->GetLineRepresentation()->SetDistanceAnnotationVisibility(true);
		this->GetLineRepresentation()->SetResolution(1);
		this->GetLineRepresentation()->GetPoint1Representation()->SetPointPlacer(this->m_pointPlacer);
		this->GetLineRepresentation()->GetPoint2Representation()->SetPointPlacer(this->m_pointPlacer);
		this->GetLineRepresentation()->GetLineHandleRepresentation()->SetPointPlacer(this->m_pointPlacer);
	}
}

void BiopsyWidget::SetWidgetSet(vtkWidgetSet2 * widgetSet)
{
	if (this->WidgetSet == widgetSet) {
		return;
	}
	if (this->WidgetSet) {
		this->WidgetSet->UnRegister(this);
	}
	this->WidgetSet = widgetSet;
	if (this->WidgetSet) {
		this->WidgetSet->Register(this);
		this->WidgetSet->AddWidget(this);
	}
}

void BiopsyWidget::SetBiopsyData(BiopsyData * data)
{
	this->m_biopsyData = data;
}

BiopsyWidget::BiopsyWidget()
{
	this->m_pointPlacer = vtkBoundedPlanePointPlacer::New();
	this->WidgetSet = nullptr;
	this->m_biopsyData = nullptr;

	vtkWidgetSet2* widgetSet = vtkWidgetSet2::New();
	this->SetWidgetSet(widgetSet);
	widgetSet->Delete();
	this->CreateDefaultRepresentation();

	this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
		vtkWidgetEvent::Move,
		this, BiopsyWidget::MoveDispatcher);
}

BiopsyWidget::~BiopsyWidget()
{
	this->m_pointPlacer->Delete();
}

void BiopsyWidget::uniqueInstall()
{	
	connect(this->ui->doubleSpinBoxRadius, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &BiopsyWidget::SetRadius);
}

void BiopsyWidget::MoveDispatcher(vtkAbstractWidget * widget)
{
	BiopsyWidget* self = static_cast<BiopsyWidget*>(widget);

	if (self->WidgetSet) {
		self->WidgetSet->DispatchAction(self, &BiopsyWidget::MoveAction);
	}
	else {
		vtkLineWidget2::MoveAction(widget);
	}
}

void BiopsyWidget::MoveAction(BiopsyWidget * dispatcher)
{
	if (this == dispatcher) {
		vtkLineWidget2::MoveAction(this);
		if (dispatcher->WidgetState == Start) {
			return;
		}
		this->GetLineRepresentation()->GetPolyData(this->m_biopsyData->getPolyData());
		double radius = reinterpret_cast<BiopsyWidgetRepresentation*>(this->GetRepresentation())->GetTubeFilter()->GetRadius();
		vtkSmartPointer<vtkDoubleArray> radiusArray = vtkSmartPointer<vtkDoubleArray>::New();
		radiusArray->InsertNextValue(radius);
		radiusArray->SetName(RADIUS.toStdString().c_str());
		this->m_biopsyData->getPolyData()->GetFieldData()->AddArray(radiusArray);
	}
	else {
		if (dispatcher->WidgetState == Start) {
			return;
		}
		double* worldPos1 = dispatcher->GetLineRepresentation()->GetPoint1WorldPosition();
		double* worldPos2 = dispatcher->GetLineRepresentation()->GetPoint2WorldPosition();
		this->GetLineRepresentation()->SetPoint1WorldPosition(worldPos1);
		this->GetLineRepresentation()->SetPoint2WorldPosition(worldPos2);
		this->Render();

	}
}
	}
}
vtkStandardNewMacro(BiopsyWidgetRepresentation);
void BiopsyWidgetRepresentation::PrintSelf(ostream & os, vtkIndent indent)
{
	vtkLineRepresentation::PrintSelf(os, indent);
}

BiopsyWidgetRepresentation::BiopsyWidgetRepresentation()
{
	this->TubeFilter = vtkTubeFilter::New();
	this->TubeFilter->SetNumberOfSides(10);
	this->TubeFilter->SetInputConnection(this->LineSource->GetOutputPort());
	this->LineMapper->SetInputConnection(this->TubeFilter->GetOutputPort());


	this->SetResolution(2);
	this->SetDirectionalLine(true);
	this->SetDistanceAnnotationVisibility(true);

}

BiopsyWidgetRepresentation::~BiopsyWidgetRepresentation()
{
	this->TubeFilter->Delete();
}