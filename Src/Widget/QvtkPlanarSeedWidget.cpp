// me
#include "ui_QvtkPlanarSeedWidget.h"
#include "QvtkPlanarSeedWidget.h"
#include "QvtkOrthogonalViewer.h"
#include "QvtkScene.h"
#include "QvtkPolyData.h"
#include "vtkWidgetSet2.h"


// vtk
#include <vtkObjectFactory.h>
#include <vtkNew.h>
#include <vtkCommand.h>
#include <vtkWidgetEvent.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkHandleWidget.h>
#include <vtkHandleRepresentation.h>
#include <vtkPolyData.h>
#include <vtkSeedRepresentation.h>
#include <vtkRenderWindowInteractor.h>

// qt
#include <QDebug>
namespace Q {
	namespace vtk {
vtkStandardNewMacro(PlanarSeedWidget);
const QString PlanarSeedWidget::SCB_PLANAR_SEED_WIDGET_DATA("SCB_PLANAR_SEED_WIDGET_DATA");
void PlanarSeedWidget::PrintSelf(ostream & os, vtkIndent indent)
{
	Superclass::PrintSelf(os, indent);
}

void PlanarSeedWidget::setCustomEnable(bool flag)
{
	UniqueUiInteractorObserver::setCustomEnable(flag);
	if (flag) {
		
		this->EnableLeftClckDropSeed(!this->getUi()->checkBoxNavigation->isChecked());
		this->SetProjectionNormal(this->getViewer()->GetOrientation());
		double* pos = this->getViewer()->GetCursorPosition();
		this->SetProjectionPosition(pos[0], pos[1], pos[2]);
		QObject::connect(this->getViewer(), &OrthogonalViewer::OrientationChanged,
			this, &PlanarSeedWidget::SetProjectionNormal);
		QObject::connect(this->getViewer(), &OrthogonalViewer::CursorPositionChanged,
			this, &PlanarSeedWidget::SetProjectionPosition);
		//	[this](double x, double y, double z) {
		//	SYNCHRONAL_CALL(
		//		PlanarSeedWidget,
		//		style->SetProjectionPosition(x, y, z);
		//	)
		//});

		Scene* scene = Scene::getCurrentScene();
		this->SeedWidgetData  = scene->getDataByUniqueName<PolyData>(SCB_PLANAR_SEED_WIDGET_DATA);

		if (!this->SeedWidgetData) {
			this->SeedWidgetData = scene->createDataByClassName<PolyData>();
			this->SeedWidgetData->setRelativePath(QStringList() << SCB_PLANAR_SEED_WIDGET_DATA + ".vtk" );
			if (!this->SeedWidgetData->getPolyData()->GetPoints()) {
				vtkNew<vtkPoints> points;
				this->SeedWidgetData->getPolyData()->SetPoints(points.GetPointer());
			}
			scene->addData(this->SeedWidgetData, SCB_PLANAR_SEED_WIDGET_DATA);
		}

	}
	else {
		this->SeedWidgetData = nullptr;

		QObject::disconnect(this->getViewer(), &OrthogonalViewer::OrientationChanged,
			this, &PlanarSeedWidget::SetProjectionNormal);
		QObject::disconnect(this->getViewer(), &OrthogonalViewer::CursorPositionChanged,
			this, &PlanarSeedWidget::SetProjectionPosition);
		// do not clean data
		Superclass::CleanAllSeed();
	}
	
}

void PlanarSeedWidget::install()
{
	UNIQUE_UI_INSTALL(PlanarSeedWidget);
	connect(this->getUi()->checkBoxNavigation, &QCheckBox::stateChanged,
		this, [this](int state) {
		EnableLeftClckDropSeed(!state);
	});


	this->SetWidgetSet(static_cast<PlanarSeedWidget*>(getUniqueThis())->GetWidgetSet());
	this->GetWidgetSet()->AddWidget(this);
}

void PlanarSeedWidget::uninstall()
{
	UNIQUE_UI_UNINSTALL();
}

void PlanarSeedWidget::SetEnabled(int i)
{
	vtkPlanarSeedWidget::SetEnabled(i);
	if (i) {
		this->LoadSeedFromPolyData(this->SeedWidgetData->getPolyData());
		this->UpdateListWidgetSeed();
	}
}

PlanarSeedWidget::PlanarSeedWidget()
{
	this->SeedWidgetData = nullptr;
	// using a NULL function to replace CompletedAction(vtkAbstractWidget *w) {};
	this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
		vtkWidgetEvent::Completed,
		this, [](vtkAbstractWidget* widget) {});
	
}

PlanarSeedWidget::~PlanarSeedWidget()
{
}

void PlanarSeedWidget::uniqueInstall()
{
	vtkNew<vtkWidgetSet2> widgetSet;
	static_cast<PlanarSeedWidget*>(this->getUniqueThis())->SetWidgetSet(widgetSet.GetPointer());

	this->SeedWidgetData = nullptr;

	//connect(this, &PlanarSeedWidget::SeedUpdated,
	//	this, &PlanarSeedWidget::UpdateListWidgetSeed);	
	connect(this->getUi()->listWidgetSeeds, &QListWidget::currentRowChanged,
		this, &PlanarSeedWidget::SetFocalSeed);
	connect(this->getUi()->pushButtonDeleteOne, &QPushButton::clicked,
		this, static_cast<void(PlanarSeedWidget::*)() > (&PlanarSeedWidget::DeleteSeed));
	connect(this->getUi()->pushButtonAddOne, &QPushButton::clicked,
		this, &PlanarSeedWidget::DropSeed);
	connect(this->getUi()->pushButtonDeleteAll, &QPushButton::clicked,
		this, &PlanarSeedWidget::CleanAllSeed);
}

void PlanarSeedWidget::EnableLeftClckDropSeed(bool flag)
{
	if (flag)
	{
		this->RestartInteraction();
	}
	else
	{
		this->CompleteInteraction();
	}
}

void PlanarSeedWidget::DropSeed()
{
	double* pos = this->getViewer()->GetCursorPosition();
	synchronalCall<PlanarSeedWidget>([&pos](PlanarSeedWidget* style) {
		style->Superclass::DropSeed(pos);
		style->Render();
	});
	//SYNCHRONAL_CALL(PlanarSeedWidget,

	//);
	UpdateListWidgetSeed();
}

void PlanarSeedWidget::AddPointAction(vtkPlanarSeedWidget * dispatcher)
{
	Superclass::AddPointAction(dispatcher);
	if (this != dispatcher) {
		return;
	}
	if (this->WidgetState == vtkSeedWidget::PlacingSeeds) {
		UpdateListWidgetSeed();
	}
}

void PlanarSeedWidget::MoveAction(vtkPlanarSeedWidget * dispatcher)
{
	Superclass::MoveAction(dispatcher);
	if (this != dispatcher) {
		return;
	}
	int X = dispatcher->GetInteractor()->GetEventPosition()[0];
	int Y = dispatcher->GetInteractor()->GetEventPosition()[1];
	int state = dispatcher->GetSeedRepresentation()->ComputeInteractionState(X, Y);
	if (state != vtkSeedRepresentation::NearSeed)
	{
		return;
	}
	UpdateListWidgetSeed();
}

void PlanarSeedWidget::DeleteAction(vtkPlanarSeedWidget * dispatcher)
{
	Superclass::DeleteAction(dispatcher);
	if (this != dispatcher) {
		return;
	}
	// Do nothing if outside
	if (this->WidgetState != vtkSeedWidget::PlacingSeeds)
	{
		return;
	}
	UpdateListWidgetSeed();
}

void PlanarSeedWidget::AddIndex(unsigned int i)
{
	if (this->m_seedIndex.contains(i)) {
		this->RemoveIndex(i);
	}
	this->m_seedIndex.push_back(i);
}

int PlanarSeedWidget::GetIndex(unsigned int i)
{
	return this->m_seedIndex.indexOf(i);
}

int PlanarSeedWidget::RemoveIndex(unsigned int i)
{
	int index = this->m_seedIndex.indexOf(i);
	if (index < 0) {
		return index;
	}
	this->m_seedIndex.removeAt(index);
	return index;
}

int PlanarSeedWidget::IndexSize()
{
	return this->m_seedIndex.size();
}

void PlanarSeedWidget::EnabledHandleInRange(vtkHandleWidget * handle)
{
	if (this->getViewer()->inherits("PlanarViewer")) {
		vtkPlanarSeedWidget::EnabledHandleInRange(handle);
	}
	else {
		handle->EnabledOn();
	}
}

void PlanarSeedWidget::UpdateListWidgetSeed()
{
	this->ui->listWidgetSeeds->clear();
	vtkNew<vtkPoints> points;
	for (vtkIdType id = 0; id < this->GetSeedRepresentation()->GetNumberOfSeeds(); ++id) {
		double pos[3];
		this->GetSeedRepresentation()->GetSeedWorldPosition(id, pos);
		points->InsertNextPoint(pos);
		this->ui->listWidgetSeeds->addItem(
			ItemTranslate(id, pos)
		);

	}

	this->SeedWidgetData->getPolyData()->SetPoints(points.GetPointer());
}

QString PlanarSeedWidget::ItemTranslate(vtkIdType id, const double pos[3])
{
	return QString(
		"Id:" + QString::number(id) + ", " +
		"X:" + QString::number(pos[0]) + ", " +
		"Y:" + QString::number(pos[1]) + ", " +
		"Z:" + QString::number(pos[2])
	);
}

void PlanarSeedWidget::DeleteSeed()
{
	int row = this->ui->listWidgetSeeds->currentRow();
	if (row < 0) {
		qWarning() << "No item is selected in listWidgetSeeds. ";
		return;
	}
	this->DeleteSeed(row);
	//SYNCHRONAL_CALL(
	//	PlanarSeedWidget,
	//	style->DeleteSeed(row));

}

void PlanarSeedWidget::DeleteSeed(int i)
{
	//SYNCHRONAL_CALL(
	//	PlanarSeedWidget, 
	//	style->Superclass::DeleteSeed(i);
	//style->Render();
	//);
	PlanarSeedWidget::synchronalCall(this, [&i](InteractorObserver *observer) {
		PlanarSeedWidget *widget = static_cast<PlanarSeedWidget*>(observer);
		widget->vtkPlanarSeedWidget::DeleteSeed(i);
		widget->Render(); });
	UpdateListWidgetSeed();
}

void PlanarSeedWidget::CleanAllSeed()
{
	SYNCHRONAL_CALL( 
		PlanarSeedWidget, 
		observer->Superclass::CleanAllSeed();
	observer->Render();
	);
	UpdateListWidgetSeed();
}

void PlanarSeedWidget::SetFocalSeed(int i)
{
	if (i < 0 || i >= this->GetSeedRepresentation()->GetNumberOfSeeds()) {
		qWarning() << "The focal seed does not exist. ";
		qWarning() << "Could not set cursor position. ";
		return;
	}
	double pos[3];
	this->GetSeedRepresentation()->GetSeedWorldPosition(i, pos);
	this->getViewer()->SetCursorPosition(pos);
	this->Render();
}

void PlanarSeedWidget::SetProjectionPosition(double x, double y, double z)
{		
	SYNCHRONAL_CALL(
		PlanarSeedWidget,
		observer->vtkPlanarSeedWidget::SetProjectionPosition(x, y, z);
	)
}

	}
}
