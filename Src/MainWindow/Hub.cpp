// me
#include "Hub.h"
#include "QvtkScene.h"
#include "QvtkImage.h"
#include "QvtkImageSlice.h"
#include "QvtkImageSliceColor.h"
#include "QvtkImageSurfaceActor.h"
#include "QvtkImageLabel2.h"
#include "QvtkOrthogonalViewer.h"
#include "QvtkVolume.h"
#include "QvtkImplant.h"
#include "QvtkPolyDataActor.h"
#include "QvtkStylesSwitchOrthogonalViewer.h"
#include "QvtkWidgetCollection.h"
#include "QvtkPolyDataSource.h"
#include "QvtkPolyDataActor.h"
#include "QvtkPolyDataActor2D.h"
#include "QvtkBiopsyData.h"
//vtk
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkMatrix3x3.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRendererCollection.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
//qt
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>
Hub* Hub::self = nullptr;
Hub::Hub(QObject* parent)
    :QObject(parent)
{
	self = this;
    this->mainWindow = new MainWindow(NUM_OF_ORTHOGONAL_VIEWER);
	Q::vtk::Scene* scene = Q::vtk::Scene::getCurrentScene();
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; ++i) {
		this->mainWindow->getViewer(i)->setEnableDepthPeeling(true);
	}
	//connection
	//QObject::connect(this->mainWindow, &MainWindow::signalImportImages, 
	//	this, &Hub::slotInitializationImages);
	QObject::connect(this->mainWindow, &MainWindow::signalImportImages,
		this, &Hub::slotInitializationImagesWithLUT);
	QObject::connect(this->mainWindow, &MainWindow::signalImportLabel,
		this, &Hub::slotInitializationLabel);
	QObject::connect(this->mainWindow, &MainWindow::signalImportModel,
		this, &Hub::slotInitializationModel);
	QObject::connect(this->mainWindow, &MainWindow::signalImportedProject,
		this, &Hub::slotImport);
	QObject::connect(this->mainWindow, &MainWindow::signalImportedProjectString,
		this, &Hub::slotImportString);
	QObject::connect(this->mainWindow, &MainWindow::signalCommitedProject,
		this, &Hub::slotCommit);
	QObject::connect(this->mainWindow->action_Clear_Project, &QAction::triggered,
		this, &Hub::slotClean);
	QObject::connect(this->mainWindow->action_Exit, &QAction::triggered,
		this, &Hub::slotExit);
	// interactorstyle
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; ++i) {
		this->styles[i] = Q::vtk::StylesSwitchOrthogonalViewer::New();
		this->styles[i]->SetViewer(this->mainWindow->getViewer(i));
		this->styles[i]->SetDefaultRenderer(this->mainWindow->getViewer(i)->getRenderers()[0]);
		this->mainWindow->getViewer(i)->getInteractor()->SetInteractorStyle(this->styles[i]);
	}
	QObject::connect(this->mainWindow->action_Testing_Mode, &QAction::toggled,
		this, &Hub::slotInteractorStyleTesting);
	QObject::connect(this->mainWindow->action_Window_Level_Mode, &QAction::toggled,
		this, &Hub::slotInteractorStyleWindowLevel);
	this->mainWindow->stackedWidgetStyle->addWidget(this->styles[0]->GetWindowLevel()->getWidget());
	QObject::connect(this->mainWindow->action_Navigation_Mode, &QAction::toggled,
		this, &Hub::slotInteractorStyleNavigation);
	this->mainWindow->stackedWidgetStyle->addWidget(this->styles[0]->GetNavigation()->getWidget());
	QObject::connect(this->mainWindow->action_Dental_Planning, &QAction::toggled,
		this, &Hub::slotInteractorStyleDentalPlanning);
	this->mainWindow->stackedWidgetStyle->addWidget(this->styles[0]->GetDentalPlanning()->getWidget());
	QObject::connect(this->mainWindow->action_Seed_Placer, &QAction::toggled,
		this, &Hub::slotInteractorStyleSeedPlacer);
	this->mainWindow->stackedWidgetStyle->addWidget(this->styles[0]->GetSeedPlacer()->getWidget());


	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; ++i) {
		this->widgets[i] = Q::vtk::WidgetCollection::New();
		this->widgets[i]->SetViewer(this->mainWindow->getViewer(i));
		this->widgets[i]->SetDefaultRenderer(this->mainWindow->getViewer(i)->getRenderers()[0]);
		this->widgets[i]->SetInteractor(this->mainWindow->getViewer(i)->getInteractor());
		this->widgets[i]->ProducePlanarSeedWidgets();
		this->widgets[i]->GetPlanarSeedWidgets()[0]->getWidget()->setEnabled(false);
	}

	QObject::connect(this->mainWindow->action_Distance_Widget_On, &QAction::triggered,
		this, &Hub::slotAddDistanceWidget);
	QObject::connect(this->mainWindow->action_Distance_Widget_Off, &QAction::triggered,
		this, &Hub::slotRemoveDistanceWidget);
	QObject::connect(this->mainWindow->action_Planar_Seed_Widget, &QAction::toggled,
		this, &Hub::slotPlanarSeedWidget);
	this->mainWindow->toolBoxWidget->addItem(this->widgets[0]->GetPlanarSeedWidgets()[0]->getWidget(), 
		this->widgets[0]->GetPlanarSeedWidgets()[0]->getWidget()->windowTitle());
	QObject::connect(this->mainWindow->action_Implant_Widget_On, &QAction::triggered,
		this, &Hub::slotAddImplantWidget);
	QObject::connect(this->mainWindow->action_Implant_Widget_Off, &QAction::triggered,
		this, &Hub::slotRemoveImplantWidget);
	QObject::connect(this->mainWindow->action_PolyData_Source_Widget_On, &QAction::triggered,
		this, &Hub::slotAddPolyDataSouceWidget);
	QObject::connect(this->mainWindow->action_PolyData_Source_Widget_Off, &QAction::triggered,
		this, &Hub::slotRemovePolyDataSourceWidget);
	scene->registerData(new Q::vtk::Implant);
	QObject::connect(this->mainWindow->action_PolyData_Source_Normal_On, &QAction::triggered,
		this, &Hub::slotAddPolyDataSouceNormal);
	QObject::connect(this->mainWindow->action_PolyData_Source_Normal_Off, &QAction::triggered,
		this, &Hub::slotRemovePolyDataSourceNormal);
	QObject::connect(this->mainWindow->action_Biopsy_On, &QAction::triggered,
		this, &Hub::slotAddBiopsyWidget);
	QObject::connect(this->mainWindow->action_Biopsy_Off, &QAction::triggered,
		this, &Hub::slotRemoveBiopsyWidget);
	scene->registerData(new Q::vtk::BiopsyData, Q::vtk::BiopsyWidget::TAG);

	QObject::connect(this->mainWindow->action_Testing_Action, &QAction::triggered,
		this, &Hub::slotTestingAction);

	this->mainWindow->show();
	this->mainWindow->action_Navigation_Mode->trigger();
}


Hub::~Hub()
{
    delete this->mainWindow;

	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; ++i) {
		this->widgets[i]->SetInteractor(nullptr);
		this->widgets[i]->Delete();
	}

	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; ++i) {
		this->styles[i]->SetInteractor(nullptr);
		this->styles[i]->Delete();
	}

}

void Hub::slotInitializationImport()
{
	using namespace Q::vtk;
	this->mainWindow->getViewer(0)->getCursorActor()->VisibilityOn();
	this->mainWindow->getViewer(1)->getCursorActor()->VisibilityOn();
	this->mainWindow->getViewer(2)->getCursorActor()->VisibilityOn();
	this->mainWindow->getViewer(3)->getCursorActor()->VisibilityOn();

	Scene* scene = Scene::getCurrentScene();
	QStringList props = scene->getAllDataByClassName("Q::vtk::Prop");

	for (QStringList::const_iterator cit = props.cbegin(); 
		cit != props.cend(); ++cit) {
		Prop* prop = static_cast<Prop*>(scene->getDataByUniqueName(*cit));
		long long id = prop->getViewerID();
		int layer = prop->getLayer();
		if (id > -1 && id < NUM_OF_ORTHOGONAL_VIEWER) {
			this->mainWindow->getViewer(id)->addProp(prop,
				this->mainWindow->getViewer(id)->getRenderers()[layer]);
		}
	}


	for (int i = 0; i < 3; ++i) {
		switch (i)
		{
		case 0:
			this->mainWindow->getViewer(i)->setOrientationToAxial();
			break;
		case 1:
			this->mainWindow->getViewer(i)->setOrientationToCoronal();
			break;
		case 2:
			this->mainWindow->getViewer(i)->setOrientationToSagittal();
			break;
		default:
			break;
		}
	}


	this->slotInitialization();
	this->slotInitializationPolyDataSourceWidget();
	this->slotInitializationPolyDataSourceNormal();
	this->slotInitializationBiopsyWidget();
	this->slotInitializationPlanarSeedWidges();
}

void Hub::slotInitialization()
{
	this->styles[0]->GetNavigation()->CentralizeCursorPosition();
	this->styles[1]->GetNavigation()->CentralizeCursorPosition();
	this->styles[2]->GetNavigation()->CentralizeCursorPosition();
	
	this->mainWindow->getViewer(0)->resetCamera(0);
	this->mainWindow->getViewer(0)->resetCameraClippingRange(0);
	this->mainWindow->getViewer(1)->resetCamera(0);
	this->mainWindow->getViewer(1)->resetCameraClippingRange(0);
	this->mainWindow->getViewer(2)->resetCamera(0);
	this->mainWindow->getViewer(2)->resetCameraClippingRange(0);
	this->mainWindow->getViewer(3)->resetCamera(0);
	this->mainWindow->getViewer(3)->resetCameraClippingRange(0);
	Q::vtk::Viewer::updateAllViewers();
}

void Hub::slotInitializationPolyDataSourceWidget()
{
	using namespace Q::vtk;
	Scene* scene = Scene::getCurrentScene();
	QStringList data = scene->getAllDataByTag("Implant");
	for (QStringList::iterator it = data.begin(); it != data.end(); ++it) {
		Implant* implant = scene->getDataByUniqueName<Implant>(*it);
		for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; ++i) {
			PolyDataSourceWidget* widget = this->widgets[i]->ProducePolyDataSourceWidgets();
			widget->SetPolyData(implant);
			this->widgets[i]->SetOneOfPolyDataSourceWidgetsEnabled(widget, true);
			if (i == 0 && this->widgets[i]->GetPolyDataSourceWidgets().size() == 1) {
				this->mainWindow->toolBoxWidget->addItem(this->widgets[0]->GetPolyDataSourceWidgets()[0]->getWidget(),
					this->widgets[0]->GetPolyDataSourceWidgets()[0]->getWidget()->windowTitle());
			}
		}

	}
}

void Hub::slotInitializationPolyDataSourceNormal()
{
	using namespace Q::vtk;
	Scene* scene = Scene::getCurrentScene();
	QStringList data = scene->getAllDataByTag("Implant");
	for (QStringList::iterator it = data.begin(); it != data.end(); ++it) {
		Implant* implant = scene->getDataByUniqueName<Implant>(*it);
		for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; ++i) {
			PolyDataSourceNormal* widget = this->widgets[i]->ProducePolyDataSourceNormals();
			widget->SetPolyData(implant);
			this->widgets[i]->SetOneOfPolyDataSourceNormalsEnabled(widget, true);
			if (i == 0 && this->widgets[i]->GetPolyDataSourceNormals().size() == 1) {
				this->mainWindow->toolBoxWidget->addItem(this->widgets[0]->GetPolyDataSourceNormals()[0]->getWidget(),
					this->widgets[0]->GetPolyDataSourceNormals()[0]->getWidget()->windowTitle());
			}
		}

	}
}

void Hub::slotInitializationBiopsyWidget()
{
	using namespace Q::vtk;
	Scene* scene = Scene::getCurrentScene();
	QStringList data = scene->getAllDataByTag(BiopsyWidget::TAG);
	vtkWidgetSet2* widgetSet = nullptr;
	for (QStringList::iterator it = data.begin(); it != data.end(); ++it) {
		PolyData* biopsydata = scene->getDataByUniqueName<PolyData>(*it);
		for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; ++i) {
			BiopsyWidget* widget = this->widgets[i]->ProduceBiopsyWidgets();
			widget->SetBiopsyData(biopsydata);
			this->widgets[i]->SetOneOfBiopsyWidgetsEnabled(widget, true);
			if (i == 0) {
				widgetSet = widget->GetWidgetSet();
				if (this->widgets[i]->GetBiopsyWidgets().size() == 1) {
					this->mainWindow->toolBoxWidget->addItem(this->widgets[0]->GetBiopsyWidgets()[0]->getWidget(),
						this->widgets[0]->GetBiopsyWidgets()[0]->getWidget()->windowTitle());
				}
			}
			else {
				widget->SetWidgetSet(widgetSet);
			}
		}

	}
}

void Hub::slotInitializationPlanarSeedWidges()
{
	using namespace Q::vtk;
	Scene* scene = Scene::getCurrentScene();
	PolyData* data = scene->getDataByUniqueName<PolyData>(PlanarSeedWidget::SCB_PLANAR_SEED_WIDGET_DATA);
	if(!data || !data->getPolyData() || !data->getPolyData()->GetPoints()){
		return;
	}
	vtkPoints* points = data->getPolyData()->GetPoints();
	//Locate image check box 
	for (vtkIdType id = 0; id < 7 && id < points->GetNumberOfPoints() ; ++id) {

		this->widgets[0]->GetPlanarSeedWidgets().last()->AddIndex(id);
	}
	
}

void Hub::slotCommit(QString path)
{
	using namespace Q::vtk;
	Scene* scene = Scene::getCurrentScene();
	scene->setSaveToXMLString(false);
	scene->setURL(path);
	scene->commit(QFileInfo(path).baseName());

}

void Hub::slotImport(QString path, bool flag)
{
	using namespace Q::vtk;
	if (flag) {
		slotClean();
	}

	Scene* scene = Scene::getCurrentScene();
	scene->setURL(path);
	scene->setLoadFromXMLString(false);
	scene->import(flag);

	slotInitializationImport();

}

void Hub::slotImportString(QString XML)
{
	using namespace Q::vtk;
	Scene* scene = Scene::getCurrentScene();
	scene->setSceneXMLString(XML);
	scene->setLoadFromXMLString(true);
	scene->import();

	slotInitializationImport();
}

void Hub::slotClean()
{
	using namespace Q::vtk;
	while (this->widgets[0]->GetDistanceWidgets().size() != 0)
	{
		this->slotRemoveDistanceWidget();
	}

	while (this->widgets[0]->GetPolyDataSourceWidgets().size() != 0)
	{
		this->slotRemovePolyDataSourceWidget();
	}

	while (this->widgets[0]->GetImplantWidgets().size() != 0) 
	{
		this->slotRemoveImplantWidget();
	}

	while (this->widgets[0]->GetBiopsyWidgets().size() != 0) {
		this->slotRemoveBiopsyWidget();
	}

	this->mainWindow->getViewer(0)->removeAllProp(this->mainWindow->getViewer(0)->getRenderers()[0]);
	this->mainWindow->getViewer(0)->removeAllProp(this->mainWindow->getViewer(0)->getRenderers()[1]);
	this->mainWindow->getViewer(1)->removeAllProp(this->mainWindow->getViewer(1)->getRenderers()[0]);
	this->mainWindow->getViewer(1)->removeAllProp(this->mainWindow->getViewer(1)->getRenderers()[1]);
	this->mainWindow->getViewer(2)->removeAllProp(this->mainWindow->getViewer(2)->getRenderers()[0]);
	this->mainWindow->getViewer(2)->removeAllProp(this->mainWindow->getViewer(2)->getRenderers()[1]);
	this->mainWindow->getViewer(3)->removeAllProp(this->mainWindow->getViewer(3)->getRenderers()[0]);
	Viewer::updateAllViewers();

	Scene::getCurrentScene()->clear();
}

void Hub::slotExit()
{
	this->mainWindow->close();
}

void Hub::slotInteractorStyleTesting(bool flag)
{
	if (!flag) {
		return;
	}
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		this->styles[i]->SetInteractorStyleToInteractorStyleTesting();
	}

}

void Hub::slotInteractorStyleWindowLevel(bool flag)
{
	if (!flag) {
		return;
	}
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		this->styles[i]->SetInteractorStyleToWindowLevel();
	}

	this->mainWindow->stackedWidgetStyle->setCurrentWidget(this->styles[0]->GetWindowLevel()->getWidget());
}

void Hub::slotInteractorStyleNavigation(bool flag)
{
	if (!flag) {
		return;
	}
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		this->styles[i]->SetInteractorStyleToNavigation();
	}
	this->mainWindow->stackedWidgetStyle->setCurrentWidget(this->styles[0]->GetNavigation()->getWidget());
	
}

void Hub::slotInteractorStyleDentalPlanning(bool flag)
{
	if (!flag) {
		return;
	}
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		this->styles[i]->SetInteractorStyleToDentalPlanning();
	}
	this->mainWindow->stackedWidgetStyle->setCurrentWidget(this->styles[0]->GetDentalPlanning()->getWidget());
}


void Hub::slotInteractorStyleSeedPlacer(bool flag)
{
	if (!flag) {
		return;
	}
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		this->styles[i]->SetInteractorStyleToSeedPlacer();
	}
	this->mainWindow->stackedWidgetStyle->setCurrentWidget(this->styles[0]->GetSeedPlacer()->getWidget());

}

void Hub::slotAddDistanceWidget()
{
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		this->widgets[i]->SetOneOfDistanceWidgetsEnabled(this->widgets[i]->ProduceDistanceWidgets(), true);
	}
}

void Hub::slotRemoveDistanceWidget()
{
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		if (this->widgets[i]->GetDistanceWidgets().size() <= 0) {
			continue;
		}
		this->widgets[i]->SetOneOfDistanceWidgetsEnabled(
			this->widgets[i]->GetDistanceWidgets().back(), false);
	}

	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		if (this->widgets[i]->GetDistanceWidgets().size() <= 0) {
			continue;
		}
		this->widgets[i]->DestroyDistanceWidgets(this->widgets[i]->GetDistanceWidgets().back());
	}
}

void Hub::slotPlanarSeedWidget(bool flag)
{
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++) {
		this->widgets[i]->SetAllPlanarSeedWidgetsEnabled(flag);
	}
	this->widgets[0]->GetPlanarSeedWidgets()[0]->getWidget()->setEnabled(flag);
}

void Hub::slotAddImplantWidget()
{
	using namespace Q::vtk;
	const double* pos = this->mainWindow->getViewer(0)->getCursorPosition();
	Scene* scene = Scene::getCurrentScene();
	Implant* implant = scene->addNewDataByClass<Implant>("ImplantWidget");
	implant->setColor(1, 0, 0);
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		ImplantWidget* widget = this->widgets[i]->ProduceImplantWidgets();
		widget->GetImplantWidgetProp()->setRenderDataSet(implant);
		this->widgets[i]->SetOneOfImplantWidgetsEnabled(widget, true);
	}
}

void Hub::slotRemoveImplantWidget()
{
	using namespace Q::vtk;
	Scene* scene = Scene::getCurrentScene();
	DataSet* implant = nullptr;
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		if (this->widgets[i]->GetImplantWidgets().size() <= 0) {
			continue;
		}
		ImplantWidget* widget = this->widgets[i]->GetImplantWidgets().last();
		implant = widget->GetImplantWidgetProp()->getRenderDataSet();
		this->widgets[i]->SetOneOfImplantWidgetsEnabled(widget, false);
	}

	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		ImplantWidget* widget = this->widgets[i]->GetImplantWidgets().last();
		this->widgets[i]->DestroyImplantWidgets(widget);
	}
	scene->removeData(implant);
}

void Hub::slotAddPolyDataSouceWidget()
{
	using namespace Q::vtk;
	const double* pos = this->mainWindow->getViewer(0)->getCursorPosition();
	Scene* scene = Scene::getCurrentScene();
	int index = scene->getAllDataByTag("Implant").size() + 1;
	Implant* implant = scene->createDataByClassName<Implant>();
	implant->setColor(1, 0, 0);
	implant->setHoverColor(0, 1, 0);
	implant->setSelectedColor(0, 0, 1);
	scene->addData(implant, "ImplantWidget");
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		PolyDataSourceWidget* widget = this->widgets[i]->ProducePolyDataSourceWidgets();
		widget->SetPolyData(implant);
		this->widgets[i]->SetOneOfPolyDataSourceWidgetsEnabled(widget, true);
		widget->PlaceWidget(
				this->mainWindow->getViewer(i)->getCursorPosition()[0],
				this->mainWindow->getViewer(i)->getCursorPosition()[1],
				this->mainWindow->getViewer(i)->getCursorPosition()[2]);
		if (i == 0 && this->widgets[i]->GetPolyDataSourceWidgets().size() == 1) {
			this->mainWindow->toolBoxWidget->addItem(this->widgets[0]->GetPolyDataSourceWidgets()[0]->getWidget(),
				this->widgets[0]->GetPolyDataSourceWidgets()[0]->getWidget()->windowTitle());
		}
	}

}

void Hub::slotRemovePolyDataSourceWidget()
{
	using namespace Q::vtk;
	Scene* scene = Scene::getCurrentScene();
	DataSet* implant = nullptr;
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		if (this->widgets[i]->GetPolyDataSourceWidgets().size() <= 0) {
			continue;
		}
		else if (i == 0) {
			implant = this->widgets[i]->GetPolyDataSourceWidgets().last()->GetPolyData();
			if (this->widgets[i]->GetPolyDataSourceWidgets().size() == 1) {
				int index = this->mainWindow->toolBoxWidget->indexOf(this->widgets[0]->GetPolyDataSourceWidgets()[0]->getWidget());
				this->mainWindow->toolBoxWidget->removeItem(index);
			}
		}
		this->widgets[i]->SetOneOfPolyDataSourceWidgetsEnabled(this->widgets[i]->GetPolyDataSourceWidgets().back(), false);
	}

	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++) {
		this->widgets[i]->DestroyPolyDataSourceWidgets();
	}

	if (implant) {
		scene->removeData(implant);
	}
}

void Hub::slotAddPolyDataSouceNormal()
{
	using namespace Q::vtk;
	const double* pos = this->mainWindow->getViewer(0)->getCursorPosition();
	Scene* scene = Scene::getCurrentScene();
	int index = scene->getAllDataByTag("Implant").size() + 1;
	Implant* implant = scene->createDataByClassName<Implant>();
	implant->setColor(1, 0, 0);
	implant->setHoverColor(0, 1, 0);
	implant->setSelectedColor(0, 0, 1);
	scene->addData(implant, "ImplantWidget");
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		PolyDataSourceNormal* widget = this->widgets[i]->ProducePolyDataSourceNormals();
		widget->SetPolyData(implant);
		this->widgets[i]->SetOneOfPolyDataSourceNormalsEnabled(widget, true);
		widget->PlaceWidget(
			this->mainWindow->getViewer(i)->getCursorPosition()[0],
			this->mainWindow->getViewer(i)->getCursorPosition()[1],
			this->mainWindow->getViewer(i)->getCursorPosition()[2]);
		if (i == 0 && this->widgets[i]->GetPolyDataSourceNormals().size() == 1) {
			this->mainWindow->toolBoxWidget->addItem(this->widgets[0]->GetPolyDataSourceNormals()[0]->getWidget(),
				this->widgets[0]->GetPolyDataSourceNormals()[0]->getWidget()->windowTitle());
		}
	}

}

void Hub::slotRemovePolyDataSourceNormal()
{
	using namespace Q::vtk;
	Scene* scene = Scene::getCurrentScene();
	DataSet* implant = nullptr;
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++)
	{
		if (this->widgets[i]->GetPolyDataSourceNormals().size() <= 0) {
			continue;
		}
		else if (i == 0) {
			implant = this->widgets[i]->GetPolyDataSourceNormals().last()->GetPolyData();
			if (this->widgets[i]->GetPolyDataSourceNormals().size() == 1) {
				int index = this->mainWindow->toolBoxWidget->indexOf(this->widgets[0]->GetPolyDataSourceNormals()[0]->getWidget());
				this->mainWindow->toolBoxWidget->removeItem(index);
			}
		}
		this->widgets[i]->SetOneOfPolyDataSourceNormalsEnabled(this->widgets[i]->GetPolyDataSourceNormals().back(), false);
	}

	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++) {
		this->widgets[i]->DestroyPolyDataSourceNormals();
	}

	if (implant) {
		scene->removeData(implant);
	}
}

void Hub::slotAddBiopsyWidget()
{
	using namespace Q::vtk;
	vtkWidgetSet2* widgetSet = nullptr;
	Scene* scene = Scene::getCurrentScene();
	PolyData *biopsyData = qobject_cast<PolyData*>(scene->addNewDataByTag(BiopsyWidget::TAG));
	biopsyData->setRelativePath(QStringList() << biopsyData->getUniqueName() + ".vtk");

	
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++) {
		BiopsyWidget* widget = this->widgets[i]->ProduceBiopsyWidgets();
		widget->SetBiopsyData(biopsyData);
		this->widgets[i]->SetOneOfBiopsyWidgetsEnabled(widget, true);
		if (i == 0) {	
			widgetSet = widget->GetWidgetSet();
			if (this->widgets[i]->GetBiopsyWidgets().size() == 1) {
				this->mainWindow->toolBoxWidget->addItem(widget->getWidget(), widget->getWidget()->windowTitle());
			}
		}
		else {
			widget->SetWidgetSet(widgetSet);
		}
	}
}

void Hub::slotRemoveBiopsyWidget()
{
	using namespace Q::vtk;
	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++) {
		if (this->widgets[i]->GetBiopsyWidgets().size() <= 0) {
			continue;
		}
		BiopsyWidget* widget = this->widgets[i]->GetBiopsyWidgets().last();
		if (i == 0) {
			if (this->widgets[i]->GetBiopsyWidgets().size() == 1) {
				int index = this->mainWindow->toolBoxWidget->indexOf(widget->getWidget());
				this->mainWindow->toolBoxWidget->removeItem(index);
			}
		}
		this->widgets[i]->SetOneOfBiopsyWidgetsEnabled(widget, false);
	}

	for (int i = 0; i < NUM_OF_ORTHOGONAL_VIEWER; i++) {
		if (this->widgets[i]->GetBiopsyWidgets().size() <= 0) {
			continue;
		}
		this->widgets[i]->DestroyBiopsyWidgets();
	}
}


void Hub::slotInitializationImages(QStringList imagePaths)
{
	using namespace Q::vtk;
	Scene* scene = Scene::getCurrentScene();

	for (QStringList::const_iterator cit = imagePaths.cbegin();
		cit != imagePaths.cend(); ++cit) {

		Image* image = scene->addNewDataByClass<Image>();
		image->setAbsolutePath(cit->split(";"));
		image->readData();
		image->setRelativePath(QStringList() << image->getUniqueName() + ".nii.gz");

		for (int i = 0; i < 3; ++i) {
			ImageSlice* imageSlice = new ImageSlice;
			imageSlice->setRenderDataSet(image);
			scene->addData(imageSlice);
			this->mainWindow->getViewer(i)->addProp(imageSlice);
			switch (i)
			{
			case 0:
				this->mainWindow->getViewer(i)->setOrientationToAxial();
				break;
			case 1:
				this->mainWindow->getViewer(i)->setOrientationToCoronal();
				break;
			case 2:
				this->mainWindow->getViewer(i)->setOrientationToSagittal();
				break;
			default:
				break;
			}
		}
		Volume* volume = new Volume;
		volume->setRenderDataSet(image);
		scene->addData(volume);
		this->mainWindow->getViewer(3)->addProp(volume);
		this->mainWindow->getViewer(3)->setOrientationToAxial();
	}

	this->slotInitialization();
}
//#include "itkImage.h"

void Hub::slotInitializationLabel(QString path)
{
	using namespace Q::vtk;
	Scene *scene = Scene::getCurrentScene();
	ImageLabel2 *label = scene->addNewDataByClass<ImageLabel2>();
	label->setAbsolutePath(QStringList() << path);
	label->readData();
	label->setRelativePath(QStringList() << label->getUniqueName() + ".nii.gz");
	label->setDefaultColorFile(19);
	for (int i = 0; i < 3; ++i) {
		ImageSlice* imageSlice = new ImageSlice;
		imageSlice->setRenderDataSet(label);
		scene->addData(imageSlice);
		this->mainWindow->getViewer(i)->addProp(imageSlice);
		switch (i)
		{
		case 0:
			this->mainWindow->getViewer(i)->setOrientationToAxial();
			break;
		case 1:
			this->mainWindow->getViewer(i)->setOrientationToCoronal();
			break;
		case 2:
			this->mainWindow->getViewer(i)->setOrientationToSagittal();
			break;
		default:
			break;
		}
	}
	
	ImageSurfaceActor *actor = new ImageSurfaceActor;
	actor->setRenderDataSet(label);
	scene->addData(actor);
	this->mainWindow->getViewer(3)->addProp(actor);
	//Image *label2 = scene->addNewDataByClass<Image>("LabelSlice");
	//label2->setAbsolutePath(QStringList() << path);
	//label2->readData();
	//label2->setRelativePath(QStringList() << label2->getUniqueName() + ".nii.gz");
	//for (int i = 0; i < 3; ++i) {
	//	ImageSliceLabel* modelSlice = new ImageSliceLabel;
	//	modelSlice->setRenderDataSet(label2);
	//	scene->addData(modelSlice);
	//	this->mainWindow->getViewer(i)->addProp(modelSlice);
	//	switch (i)
	//	{
	//	case 0:
	//		this->mainWindow->getViewer(i)->setOrientationToAxial();
	//		break;
	//	case 1:
	//		this->mainWindow->getViewer(i)->setOrientationToCoronal();
	//		break;
	//	case 2:
	//		this->mainWindow->getViewer(i)->setOrientationToSagittal();
	//		break;
	//	default:
	//		break;
	//	}
	//}
	this->slotInitialization();
}

void Hub::slotInitializationImagesWithLUT(QStringList imagePaths)
{
	using namespace Q::vtk;
	Scene* scene = Scene::getCurrentScene();

	for (QStringList::const_iterator cit = imagePaths.cbegin();
		cit != imagePaths.cend(); ++cit) {

		Image* image = scene->addNewDataByClass<Image>();
		image->setAbsolutePath(cit->split(";"));
		image->readData();
		image->setRelativePath(QStringList() << image->getUniqueName() + ".nii.gz");

		for (int i = 0; i < 3; ++i) {
			ImageSliceColor* imageSlice = new ImageSliceColor;
			imageSlice->setRenderDataSet(image);
			scene->addData(imageSlice);
			this->mainWindow->getViewer(i)->addProp(imageSlice);
			switch (i)
			{
			case 0:
				this->mainWindow->getViewer(i)->setOrientationToAxial();
				break;
			case 1:
				this->mainWindow->getViewer(i)->setOrientationToCoronal();
				break;
			case 2:
				this->mainWindow->getViewer(i)->setOrientationToSagittal();
				break;
			default:
				break;
			}
		}
		Volume* volume = new Volume;
		volume->setRenderDataSet(image);
		scene->addData(volume);
		this->mainWindow->getViewer(3)->addProp(volume);
		this->mainWindow->getViewer(3)->setOrientationToAxial();
	}

	this->slotInitialization();
}

void Hub::slotInitializationModel(QString path)
{
	using namespace Q::vtk;
	Scene *scene = Scene::getCurrentScene();
	PolyData *model = scene->createDataByClassName<PolyData>();
	model->setAbsolutePath(path);
	model->readData();
	model->setRelativePath(model->getUniqueName() + ".vtk");
	model->setColor(1, 0, 0);
	scene->addData(model);
	for (int i = 0; i < 3; ++i) {
		PolyDataActor2D *modelSlice = new PolyDataActor2D;
		modelSlice->setRenderDataSet(model);
		scene->addData(modelSlice);
		this->mainWindow->getViewer(i)->addProp(modelSlice);
		switch (i)
		{
		case 0:
			this->mainWindow->getViewer(i)->setOrientationToAxial();
			break;
		case 1:
			this->mainWindow->getViewer(i)->setOrientationToCoronal();
			break;
		case 2:
			this->mainWindow->getViewer(i)->setOrientationToSagittal();
			break;
		default:
			break;
		}
	}

	PolyDataActor *actor = new PolyDataActor;
	actor->setRenderDataSet(model);
	scene->addData(actor);
	this->mainWindow->getViewer(3)->addProp(actor);
	this->slotInitialization();
}

void Hub::slotTestingAction()
{
	//using namespace Q::vtk;
	//NeuralTube *tube = new NeuralTube;
	//tube->setProperty("tubeRadius", 5.0);
	//tube->setProperty("tubeLength", 200);
	//tube->setProperty("intermediatePoint", QVariantList() << -46.71 << -70.75 << 74.52);
	//tube->setProperty("endPoint", QVariantList() << 5.14 << -14.82 << 39.52);
	//tube->setColor(1, 1, 1);
	////tube->setOpacity(0.3);

	//PolyDataActor *tubeActor = new PolyDataActor;
	//tubeActor->setRenderDataSet(tube);
	//this->mainWindow->getViewer(3)->addProp(tubeActor);

	//Implant* implant = new Implant;
	//implant->setColor(1, 0, 0);
	////implant->setOpacity(0.3);
	//implant->getUserMatrix()->DeepCopy(tube->getAdditionalMatrix());
	//cout << *tube->getAdditionalMatrix() << '\n';

	//PolyDataActor *implantActor = new PolyDataActor;
	//implantActor->setRenderDataSet(implant);
	//this->mainWindow->getViewer(3)->addProp(implantActor);
}