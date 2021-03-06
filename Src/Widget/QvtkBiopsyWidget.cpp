// me
#include "QvtkBiopsyWidget.h"
#include "QvtkProp.h"
#include "QvtkImageSlice.h"
#include "QvtkOrthogonalViewer.h"
#include "QvtkBiopsyData.h"
#include "QvtkPolyData.h"
#include "vtkWidgetSet2.h"
#include "ui_QvtkBiopsyWidget.h"
#include "QvtkScene.h"
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
#include <vtkPlane.h>
// qt
#include <QDebug>


namespace Q {
	namespace vtk {


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

		vtkStandardNewMacro(BiopsyWidget);
		const QString BiopsyWidget::RADIUS("RADIUS");
		const QString BiopsyWidget::NORMAL("NORMAL");
		const QString BiopsyWidget::TAG("Biopsy tube");
		void BiopsyWidget::PrintSelf(ostream & os, vtkIndent indent)
		{
			vtkLineWidget2::PrintSelf(os, indent);
		}

		void BiopsyWidget::setCustomEnable(bool flag)
		{
			UniqueUiInteractorObserver::setCustomEnable(flag);
			if (flag) {
				const QList<Prop*>& props = this->getViewer()->getProps();

				this->SetProjectionNormal(this->getViewer()->getOrientation());
				const double* pos = this->getViewer()->getCursorPosition();
				this->SetProjectionPosition(pos[0], pos[1], pos[2]);

				QObject::connect(this->getViewer(), &OrthogonalViewer::OrientationChanged,
					this, &BiopsyWidget::SetProjectionNormal);
				QObject::connect(this->getViewer(), &OrthogonalViewer::cursorPositionChanged,
					this, &BiopsyWidget::SetProjectionPosition);
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
				if (this->m_biopsyData->getPolyData()->GetFieldData()->GetArray(RADIUS.toStdString().c_str())) {
					double radius =
						this->m_biopsyData->getPolyData()->GetFieldData()->GetArray(RADIUS.toStdString().c_str())->GetTuple1(0);
					this->SetRadius(radius);
				}
				else {
					this->SetRadius(2);
				}
				//QObject::connect(this->ui->doubleSpinBoxRadius, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
				//	this->m_biopsyData, &BiopsyData::setRadius);
			}
			else {
				QObject::disconnect(this->getViewer(), &OrthogonalViewer::OrientationChanged,
					this, &BiopsyWidget::SetProjectionNormal);
				QObject::disconnect(this->getViewer(), &OrthogonalViewer::cursorPositionChanged,
					this, &BiopsyWidget::SetProjectionPosition);
				if (this->m_biopsyData) {
					//QObject::connect(this->ui->doubleSpinBoxRadius, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
					//	this->m_biopsyData, &BiopsyData::setRadius);
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
			switch (normal)
			{
			case OrthogonalViewer::ORIENTATION_YZ:
			case OrthogonalViewer::ORIENTATION_XZ:
			case OrthogonalViewer::ORIENTATION_XY:
				this->m_pointPlacer->SetProjectionNormal(normal);
				break;
			case OrthogonalViewer::SAGITTAL:
			case OrthogonalViewer::CORONAL:
			case OrthogonalViewer::AXIAL:
			default: {
				this->m_pointPlacer->SetProjectionNormalToOblique();
				this->m_pointPlacer->GetObliquePlane()->SetNormal(
					this->getViewer()->getCurrentPlaneNormal()[0],
					this->getViewer()->getCurrentPlaneNormal()[1],
					this->getViewer()->getCurrentPlaneNormal()[2]);
				break;
			}
			}
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
				case vtkBoundedPlanePointPlacer::Oblique:
					widget->m_pointPlacer->GetObliquePlane()->SetOrigin(x, y, z);
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
			vtkSmartPointer<vtkDoubleArray> radiusArray = vtkSmartPointer<vtkDoubleArray>::New();
			radiusArray->InsertNextValue(radius);
			radiusArray->SetName(RADIUS.toStdString().c_str());
			this->m_biopsyData->getPolyData()->GetFieldData()->AddArray(radiusArray);
		}

		void BiopsyWidget::CreateDefaultRepresentation()
		{
			if (!this->WidgetRep) {

				BiopsyWidgetRepresentation* rep = BiopsyWidgetRepresentation::New();
				this->SetWidgetRepresentation(rep);
				rep->Delete();
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

		void BiopsyWidget::SetBiopsyData(PolyData * data)
		{
			this->m_biopsyData = data;
		}

		BiopsyWidget::BiopsyWidget()
		{
			this->m_pointPlacer = vtkBoundedPlanePointPlacer::New();
			this->m_pointPlacer->SetObliquePlane(vtkSmartPointer<vtkPlane>::New());
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
