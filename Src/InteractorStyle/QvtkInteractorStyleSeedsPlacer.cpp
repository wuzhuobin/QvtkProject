// me
#include "QvtkInteractorStyleSeedsPlacer.h"
#include "QvtkScene.h"
#include "QvtkPolyData.h"
#include "QvtkInteractorStyleNavigation.h"
#include "QvtkOrthogonalViewer.h"
#include "ui_QvtkInteractorStyleSeedsPlacer.h"

// vtk 
#include <vtkSeedWidget.h>
#include <vtkSeedRepresentation.h>
#include <vtkPointHandleRepresentation3D.h>
#include <vtkBoundedPlanePointPlacer.h>
#include <vtkNew.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkHandleWidget.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>
#include <vtkCallbackCommand.h>

// qt
#include <QDebug>

namespace Q {
	namespace vtk {

class InteractorStyleSeedsPlacerWidget : public vtkSeedWidget
{
public:
	static InteractorStyleSeedsPlacerWidget* New() { return new InteractorStyleSeedsPlacerWidget; }
	vtkTypeMacro(InteractorStyleSeedsPlacerWidget, vtkSeedWidget);

	InteractorStyleSeedsPlacerWidget() {
		// using a NULL function to replace CompletedAction(vtkAbstractWidget *w) {};
		this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
			vtkWidgetEvent::Completed,
			this, [](vtkAbstractWidget* widget) {});
	}

	//class SCBInteractorStyleSeedsPlacerWidgetCallback : public vtkCallbackCommand
	//{
	//	public:
	//		vtkTypeMacro(SCBInteractorStyleSeedsPlacerWidgetCallback, vtkCallbackCommand);
	//		static SCBInteractorStyleSeedsPlacerWidgetCallback* New() { return new SCBInteractorStyleSeedsPlacerWidgetCallback; }
	//		SCBInteractorStyleSeedsPlacerWidget* self;
	//		virtual void Execute(vtkObject *caller, unsigned long event,
	//			void *callData) {

	//		}

	//};

};

const QString InteractorStyleSeedsPlacer::SEEDS_PLACER_DATA("SEEDS_FOR_SEEDS_PLACER");

vtkStandardNewMacro(InteractorStyleSeedsPlacer);
void InteractorStyleSeedsPlacer::SetInteractor(vtkRenderWindowInteractor * interactor)
{
	InteractorStyleNavigation::SetInteractor(interactor);
	this->SeedsWidget->SetInteractor(interactor);
}

//void SCBInteractorStyleSeedsPlacer::OnChar()
//{
//	char c = this->Interactor->GetKeyCode();
//	vtkPoints* points = this->SeedsData->getPolyData()->GetPoints();
//	switch (c)
//	{
//	case 'h':
//		qDebug() << "seeds";
//		for (int i = 0; i < this->SeedsWidget->GetSeedRepresentation()->GetNumberOfSeeds(); ++i) {
//			double pos[3];
//			this->SeedsWidget->GetSeedRepresentation()->GetSeedWorldPosition(i, pos);
//			qDebug() << i;
//			qDebug() << pos[0];
//			qDebug() << pos[1];
//			qDebug() << pos[2];
//
//		}
//		qDebug() << "points";
//		for (int i = 0; i < points->GetNumberOfPoints(); ++i) {
//			qDebug() << i;
//			qDebug() << points->GetPoint(i)[0];
//			qDebug() << points->GetPoint(i)[1];
//			qDebug() << points->GetPoint(i)[2];
//		}
//
//	default:
//		break;
//	}
//}

void InteractorStyleSeedsPlacer::SetCursorPosition(double x, double y, double z)
{
	if (this->ui->doubleSpinBoxX->value() == x &&
		this->ui->doubleSpinBoxY->value() == y &&
		this->ui->doubleSpinBoxZ->value() == z) {
		return;
	}
	this->ui->doubleSpinBoxX->blockSignals(true);
	this->ui->doubleSpinBoxY->blockSignals(true);
	this->ui->doubleSpinBoxZ->blockSignals(true);
	this->m_viewer->SetCursorPosition(x, y, z);

	if (x < this->ui->doubleSpinBoxX->minimum()) {
		this->ui->doubleSpinBoxX->setMinimum(x);
	}
	if (x > this->ui->doubleSpinBoxX->maximum()) {
		this->ui->doubleSpinBoxX->setMaximum(x);
	}
	this->ui->doubleSpinBoxX->setValue(x);

	if (y > this->ui->doubleSpinBoxY->maximum()) {
		this->ui->doubleSpinBoxY->setMaximum(y);
	}
	if (y < this->ui->doubleSpinBoxY->minimum()) {
		this->ui->doubleSpinBoxY->setMinimum(y);
	}
	this->ui->doubleSpinBoxY->setValue(y);

	if (z > this->ui->doubleSpinBoxZ->maximum()) {
		this->ui->doubleSpinBoxZ->setMaximum(z);
	}
	if (z < this->ui->doubleSpinBoxZ->minimum()) {
		this->ui->doubleSpinBoxZ->setMinimum(z);
	}
	this->ui->doubleSpinBoxZ->setValue(z);

	this->ui->doubleSpinBoxX->blockSignals(false);
	this->ui->doubleSpinBoxY->blockSignals(false);
	this->ui->doubleSpinBoxZ->blockSignals(false);

	SynRefresh();

}

void InteractorStyleSeedsPlacer::SetProjectionNormal(int normal)
{
	this->PointPlacer->SetProjectionNormal(normal);
}

void InteractorStyleSeedsPlacer::PrintSelf(ostream & os, vtkIndent indent)
{
	InteractorStyleNavigation::PrintSelf(os, indent);
}

void InteractorStyleSeedsPlacer::setCustomEnable(bool flag)
{
	InteractorStyleNavigation::setCustomEnable(flag);
	this->SeedsWidget->SetEnabled(flag);
	Scene* scene = Scene::getCurrentScene();
	this->SeedsData = qobject_cast<PolyData*>(scene->getDataByUniqueName(SEEDS_PLACER_DATA));
	if (!this->SeedsData) {
		this->SeedsData = qobject_cast<PolyData*>(scene->createDataByClassName("Q::vtk::PolyData"));
		this->SeedsData->setRelativePath(QStringList() << SEEDS_PLACER_DATA + ".vtp");
		vtkNew<vtkPoints> points;
		this->SeedsData->getPolyData()->SetPoints(points.GetPointer());
		scene->addData(this->SeedsData, SEEDS_PLACER_DATA);
	}
	if (flag) {

		SetProjectionNormal(getViewer()->GetOrientation());
		SetProjectionPosition(
			this->m_viewer->getCursorPosition()[0], 
			this->m_viewer->getCursorPosition()[1],
			this->m_viewer->getCursorPosition()[2]);

		GenerateWidgetFromSeedsData();
		this->SeedsWidget->Render();

		connect(this->m_viewer, &OrthogonalViewer::OrientationChanged,
			this, &InteractorStyleSeedsPlacer::SetProjectionNormal);
		connect(this->m_viewer, &OrthogonalViewer::CursorPositionChanged,
			this, &InteractorStyleSeedsPlacer::SetProjectionPosition);
		EnableNavigation(this->NavigationFlag);
	}
	else
	{
		ClearSeedsWidget();
		this->SeedsWidget->Render();
		disconnect(this->m_viewer, &OrthogonalViewer::OrientationChanged,
			this, &InteractorStyleSeedsPlacer::SetProjectionNormal);
		disconnect(this->m_viewer, &OrthogonalViewer::CursorPositionChanged,
			this, &InteractorStyleSeedsPlacer::SetProjectionPosition);
	}
}

void InteractorStyleSeedsPlacer::install()
{
	UNIQUE_UI_INSTALL(InteractorStyleSeedsPlacer);
	connect(this->ui->checkBoxNavigation, &QCheckBox::stateChanged,
		this, &InteractorStyleSeedsPlacer::EnableNavigation);


}

void InteractorStyleSeedsPlacer::uninstall()
{
	UNIQUE_UI_UNINSTALL();
}

void InteractorStyleSeedsPlacer::SetProjectionPosition(double x, double y, double z)
{
	switch (this->PointPlacer->GetProjectionNormal())
	{
	case vtkBoundedPlanePointPlacer::XAxis:
		this->PointPlacer->SetProjectionPosition(x);
		break;
	case vtkBoundedPlanePointPlacer::YAxis:
		this->PointPlacer->SetProjectionPosition(y);
		break;
	case vtkBoundedPlanePointPlacer::ZAxis:
		this->PointPlacer->SetProjectionPosition(z);
		break;
	default:
		break;
	}
}

void InteractorStyleSeedsPlacer::EnableNavigation(bool flag)
{
	this->NavigationFlag = flag;
	if (flag) {
		this->SeedsWidget->CompleteInteraction();
	}
	else {
		this->SeedsWidget->RestartInteraction();
	}
}

void InteractorStyleSeedsPlacer::AddOne()
{
	const double* seed = this->m_viewer->getCursorPosition();
	vtkHandleWidget* newSeed = this->SeedsWidget->CreateNewHandle();
	newSeed->GetHandleRepresentation()->SetWorldPosition(const_cast<double*>(seed));
	newSeed->SetInteractor(this->Interactor);
	newSeed->EnabledOn();
	UpdateSeedsData(seed, seed);
}

void InteractorStyleSeedsPlacer::FocalOne(int id)
{
	vtkPoints* seeds = this->SeedsData->getPolyData()->GetPoints();
	if (id < 0 || id >= seeds->GetNumberOfPoints()) {
		return;
	}
	const double* pos = seeds->GetPoint(id);
	SetCursorPosition(pos[0], pos[1], pos[2]);
}

void InteractorStyleSeedsPlacer::DeleteOne()
{
	int row = this->ui->listWidgetSeeds->currentRow();
	if (row < 0) {
		qWarning() << "No item is selected in listWidgetSeeds. ";
		return;
	}
	DeleteOne(row);

}

void InteractorStyleSeedsPlacer::DeleteOne(int id)
{
	vtkPoints* seeds = this->SeedsData->getPolyData()->GetPoints();
	double pos[3];
	seeds->GetPoint(id, pos);
	UpdateSeedsData(pos, nullptr);
	SynRefresh();
}

void InteractorStyleSeedsPlacer::DeleteAll()
{
	vtkPoints* seeds = this->SeedsData->getPolyData()->GetPoints();
	seeds->Reset();
	seeds->Squeeze();
	this->ui->listWidgetSeeds->clear();
	SynRefresh();
}

void InteractorStyleSeedsPlacer::GenerateWidgetFromSeedsData()
{
	//ClearSeedsWidget();
	vtkPoints* seeds = this->SeedsData->getPolyData()->GetPoints();

	const double* pos = this->m_viewer->getCursorPosition();
	int orientation = this->m_viewer->GetOrientation();

	for (vtkIdType id = 0; id < seeds->GetNumberOfPoints(); ++id) {
		double* seed = seeds->GetPoint(id);


		if (this->getViewer()->inherits("PlanarViewer")) {
			if (qAbs(pos[orientation] - seed[orientation]) > this->DisplayThickness) {
				continue;

			}
		}

		vtkHandleWidget* newSeed = this->SeedsWidget->CreateNewHandle();
		newSeed->GetHandleRepresentation()->SetWorldPosition(seed);
		newSeed->EnabledOn();

	}


}

void InteractorStyleSeedsPlacer::SaveWidgetToSeedData()
{
	for (int i = this->SeedsWidget->GetSeedRepresentation()->GetNumberOfSeeds() - 1; i >= 0; --i) {
		//double* worldPos = new double[3]; // #MemLeakHere
		double worldPos[3];
		this->SeedsWidget->GetSeedRepresentation()->GetSeedWorldPosition(i, worldPos);

		UpdateSeedsData(worldPos, worldPos);

	}
}

void InteractorStyleSeedsPlacer::SynRefresh()
{
	SYNCHRONAL_CALL(
		InteractorStyleSeedsPlacer,
		observer->ClearSeedsWidget();
	observer->GenerateWidgetFromSeedsData();
	observer->SeedsWidget->Render();
	);
}

void InteractorStyleSeedsPlacer::ClearSeedsWidget()
{
	// The seed of vtkSeedWidget begins from the rear
	for (int i = this->SeedsWidget->GetSeedRepresentation()->GetNumberOfSeeds() - 1; i >= 0; --i) {
		this->SeedsWidget->DeleteSeed(i);
	}
}

double cachedOldPos[3] = { VTK_DOUBLE_MAX, VTK_DOUBLE_MAX, VTK_DOUBLE_MAX };

InteractorStyleSeedsPlacer::InteractorStyleSeedsPlacer()
{
	this->DisplayThickness = 0.5;
	this->NavigationFlag = false;

	vtkNew<vtkBoundedPlanePointPlacer> pointPlacer;
	this->PointPlacer = pointPlacer.GetPointer();

	vtkNew<vtkPointHandleRepresentation3D> handleRep;
	handleRep->SetPointPlacer(this->PointPlacer);

	this->SeedsWidget = InteractorStyleSeedsPlacerWidget::New();
	this->SeedsWidget->CreateDefaultRepresentation();
	this->SeedsWidget->GetSeedRepresentation()->SetHandleRepresentation(handleRep.GetPointer());

	vtkNew<vtkCallbackCommand> saveCallback;
	saveCallback->SetClientData(this);
	saveCallback->SetCallback([](vtkObject *caller, unsigned long eid,
		void *clientdata, void *calldata) {
		InteractorStyleSeedsPlacer* self = 
			reinterpret_cast<InteractorStyleSeedsPlacer*>(clientdata);
		self->SaveWidgetToSeedData();
		self->SynRefresh();

	});
	this->SeedsWidget->AddObserver(vtkCommand::PlacePointEvent, saveCallback.GetPointer());

	vtkNew<vtkCallbackCommand> modifyCallback;
	modifyCallback->SetClientData(this);
	modifyCallback->SetCallback([](vtkObject *caller, unsigned long eid,
		void *clientdata, void *calldata) {

		vtkSeedWidget* seedWidget = static_cast<vtkSeedWidget*>(caller);
		int seedNum = seedWidget->GetSeedRepresentation()->GetActiveHandle();
		double activePos[3];
		seedWidget->GetSeedRepresentation()->GetSeedWorldPosition(seedNum, activePos);
		InteractorStyleSeedsPlacer* self =
			reinterpret_cast<InteractorStyleSeedsPlacer*>(clientdata);
		if (eid == vtkCommand::StartInteractionEvent) {
			// save the initial position
			memcpy(cachedOldPos, activePos, sizeof(activePos));
		}
		else if (eid == vtkCommand::EndInteractionEvent) {
			// update the positon
			self->UpdateSeedsData(cachedOldPos, activePos);
			self->SynRefresh();
		}
	});
	this->SeedsWidget->AddObserver(vtkCommand::StartInteractionEvent, modifyCallback.GetPointer());
	this->SeedsWidget->AddObserver(vtkCommand::EndInteractionEvent, modifyCallback.GetPointer());


	this->SeedsData = nullptr;
}

InteractorStyleSeedsPlacer::~InteractorStyleSeedsPlacer()
{
	this->SeedsWidget->Delete();
}

void InteractorStyleSeedsPlacer::uniqueInstall()
{
	// here this == uniqueThis
	connect(this->ui->doubleSpinBoxX, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &InteractorStyleNavigation::SetCursorPositionX);
	connect(this->ui->doubleSpinBoxY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &InteractorStyleNavigation::SetCursorPositionY);
	connect(this->ui->doubleSpinBoxZ, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &InteractorStyleNavigation::SetCursorPositionZ);

	connect(this->ui->listWidgetSeeds, &QListWidget::currentRowChanged,
		this, &InteractorStyleSeedsPlacer::FocalOne); 
	connect(this->ui->pushButtonAddOne, &QPushButton::clicked,
		this, &InteractorStyleSeedsPlacer::AddOne);
	connect(this->ui->pushButtonDeleteOne, &QPushButton::clicked,
		this, static_cast<void(InteractorStyleSeedsPlacer::*)()>(&InteractorStyleSeedsPlacer::DeleteOne));
	connect(this->ui->pushButtonDeleteAll, &QPushButton::clicked,
		this, &InteractorStyleSeedsPlacer::DeleteAll);

}
//
//void SCBInteractorStyleSeedsPlacer::uniqueEnable()
//{
//	InteractorStyleNavigation::uniqueEnable();
//}
//
//void SCBInteractorStyleSeedsPlacer::uniqueDisable()
//{
//	InteractorStyleNavigation::uniqueDisable();
//}

QString InteractorStyleSeedsPlacer::ItemTranslate(vtkIdType id, const double pos[3])
{
	return QString(
		"Id:" + QString::number(id) + ", " +
		"X:" + QString::number(pos[0]) + ", " +
		"Y:" + QString::number(pos[1]) + ", " +
		"Z:" + QString::number(pos[2])
	);
}

void InteractorStyleSeedsPlacer::UpdateSeedsData(const double* oldPos, const double* newPos)
{

	vtkPoints* seeds = this->SeedsData->getPolyData()->GetPoints();
	if (seeds->GetNumberOfPoints() != 0) {
		
		for (vtkIdType id = 0; id < seeds->GetNumberOfPoints(); ++id) {
			// find the matched point and replace it
			if (vtkMath::Distance2BetweenPoints(seeds->GetPoint(id), oldPos) < 0.005) {
				//qDebug() << "Distancce";
				//qDebug() << vtkMath::Distance2BetweenPoints(seeds->GetPoint(id), oldPos);
				//qDebug() << "oldPos";
				//qDebug() << oldPos[0];
				//qDebug() << oldPos[1];
				//qDebug() << oldPos[2];
				//qDebug() << "point";
				//qDebug() << seeds->GetPoint(id)[0];
				//qDebug() << seeds->GetPoint(id)[1];
				//qDebug() << seeds->GetPoint(id)[2];
				if (newPos) {
					seeds->SetPoint(id, newPos);
					this->ui->listWidgetSeeds->item(id)->setText(ItemTranslate(id + 1, newPos));

				}
				else {
					// deleting is complicated.
					vtkNew<vtkPoints> newSeeds;
					for (vtkIdType id2 = 0; id2 < seeds->GetNumberOfPoints(); ++id2) {
						if (id2 != id) {
							newSeeds->InsertNextPoint(seeds->GetPoint(id2));
						}
					}
					seeds->ShallowCopy(newSeeds.GetPointer());
					delete this->ui->listWidgetSeeds->takeItem(id);
				}
				return;

			}
		}


	}
	// if no match point was found, insert one
	seeds->InsertNextPoint(newPos);
	this->ui->listWidgetSeeds->addItem(ItemTranslate(seeds->GetNumberOfPoints(), newPos));
}

	}
}
