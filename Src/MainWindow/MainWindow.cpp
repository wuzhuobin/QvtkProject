// me
#include "MainWindow.h"
#include "MedicalImageFinder.h"
#include "QvtkPlanarViewer.h"
#include "QvtkNonPlanarViewer.h"

// vtk
#include <vtkRenderer.h>

// qt
#include <QFileDialog>
#include <QSettings>
#include <QActionGroup>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

const QString MainWindow::PASSWORD("WUZHUOBIN");

MainWindow::MainWindow(int numOfViewers, QWidget * parent)
	:QMainWindow(parent),
	stylesGroup(this)
{
	this->setupUi(this);

	// dock viewers
	for (int i = 0; i < numOfViewers - 1; ++i) {
		Q::vtk::PlanarViewer *planarViewer = new Q::vtk::PlanarViewer(&this->viewers);
		planarViewer->updateAxesFlagOn();
		planarViewer->orientationTextFlagOn();
		this->viewers.setViewer(i, planarViewer);
	}
	Q::vtk::NonPlanarViewer *nonPlanarViewer = new Q::vtk::NonPlanarViewer(&this->viewers);
	nonPlanarViewer->orientationMarkerWidgetFlagOn();
	this->viewers.setViewer(numOfViewers - 1, nonPlanarViewer);
	this->verticalLayoutOrthogonal->insertWidget(0, &this->viewers);
	this->dockWidgetScene->setWidget(&this->sceneWidget);
	// interactorstyles
	this->stylesGroup.addAction(this->action_Testing_Mode);
	this->stylesGroup.addAction(this->action_Navigation_Mode);
	this->stylesGroup.addAction(this->action_Window_Level_Mode);
	this->stylesGroup.addAction(this->action_Dental_Planning);
	this->stylesGroup.addAction(this->action_Seed_Placer);
	this->stylesGroup.addAction(this->action_Micron_Tracker_Navigation);
	this->stylesGroup.setExclusive(true);

	// connection
	// menu file
	QObject::connect(this->action_Import_Images_From_Medical_Image_Finder, SIGNAL(triggered()),
		this, SLOT(slotImportImagesFromMedicalImageFinder()));
	QObject::connect(this->action_Import_Images_From_Database, &QAction::triggered,
		this, &MainWindow::slotImportImagesFromDatabase);
	QObject::connect(this->action_Commit_Project, SIGNAL(triggered()),
		this, SLOT(slotCommitProject()));
	QObject::connect(this->action_Import_Project, SIGNAL(triggered()),
		this, SLOT(slotImportProject()));
	QObject::connect(this->action_Import_Project_Without_Clean, &QAction::triggered,
		this, [this]() {this->slotImportProject(QString(), false);});
	QObject::connect(this->action_Import_Project_String, SIGNAL(triggered()),
		this, SLOT(slotImportProjectString()));
	QObject::connect(&this->databaseWidget, &ctkDICOMAppWidget2::imageFilesSent,
		this, &MainWindow::signalImportImages);
	QObject::connect(this->action_Import_Images, SIGNAL(triggered()), 
		this, SLOT(slotImportImages()));
	QObject::connect(this->action_Import_Label, SIGNAL(triggered()),
		this, SLOT(slotImportLabel()));
	QObject::connect(this->action_Import_Model, SIGNAL(triggered()),
		this, SLOT(slotImportModel()));
	// for recent images used
	this->settings = new QSettings("Setting.ini", QSettings::IniFormat, this);
	createRecentImageActions();

}

MainWindow::~MainWindow()
{
}

Q::vtk::OrthogonalViewer * MainWindow::getViewer(int i)
{
	return this->viewers.getViewers(i);
}

void MainWindow::slotImportImages(QStringList paths)
{
	paths = QFileDialog::getOpenFileNames(
		this,
		"Import Images",
		paths.isEmpty() ? QString() : paths.first(),
		"Images (* *.nii *.nii.gz)");
	if (!paths.isEmpty()) {
		emit signalImportImages(QStringList() << paths.join(';'));
	}
}

void MainWindow::slotImportImagesFromMedicalImageFinder(QString path)
{
	MedicalImageFinder rw(path, 4);
	rw.setImageModalityNames(0, "Image 0");
	rw.setImageModalityNames(1, "Image 1");
	rw.setImageModalityNames(2, "Image 2");
	rw.setImageModalityNames(3, "Image 3");

	if (QWizard::Accepted == rw.exec()) {
		adjustForCurrentFile(rw.getDirectory());
		emit signalImportImages(rw.getFileNames());
	}
}
void MainWindow::slotImportImagesFromDatabase()
{
	this->databaseWidget.show();
	this->databaseWidget.raise();
}
void MainWindow::slotRecentImagesFromMedicalImageFinder()
{
	QCoreApplication::processEvents();

	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
	{
		slotImportImagesFromMedicalImageFinder(action->data().toString());
	}
}
void MainWindow::slotImportLabel(QString path)
{
	path = QFileDialog::getOpenFileName(
		this, 
		"Import Label",
		path, 
		"Label(*.nii *.nii.gz)");
	if (!path.isEmpty()) {
		emit signalImportLabel(path);
	}
}
void MainWindow::slotImportModel(QString path)
{
	path = QFileDialog::getOpenFileName(
		this,
		"Import Model",
		path,
		"Stereolithography(*.stl);; VTK PolyData(*.vtk);; VTP PolyData(*.vtp)");
	if (!path.isEmpty()) {
		emit signalImportModel(path);
	}
	
}
void MainWindow::slotImportProject(QString path, bool clean)
{
	QString project = QFileDialog::getOpenFileName(this, tr("Import XML"),
		path, tr("XML files (*.xml)"));
	if (project.isEmpty()) {
		return;
	}
	emit signalImportedProject(project, clean);
}

void MainWindow::slotImportProjectString(QString xml)
{
	if (xml.isEmpty()) {
		bool ok = false;
		xml = QInputDialog::getMultiLineText(this, tr("Import XML"),
			tr("XML Content:"), xml, &ok);
		if (!ok) {
			return;
		}
	}
	emit signalImportedProjectString(xml);
}

void MainWindow::slotCommitProject(QString path)
{
    QString project = QFileDialog::getSaveFileName(this, tr("Commit XML"),
                                                   path, tr("XML files (*.xml)"));
    if (project.isEmpty()) {
        return;
    }
    emit signalCommitedProject(project);
}

void MainWindow::closeEvent(QCloseEvent* closeEvent)
{
	this->action_Exit->triggered();
	QMainWindow::closeEvent(closeEvent);
}

void MainWindow::createRecentImageActions()
{
	QAction* recentFileAction = 0;
	for (int i = 0; i < MAX_RECENT_IMAGE; i++) {
		recentFileAction = new QAction(this);
		recentFileAction->setVisible(false);
		QObject::connect(recentFileAction, SIGNAL(triggered()), this, SLOT(slotRecentImagesFromMedicalImageFinder()));

		recentFileActionList.append(recentFileAction);
		this->menu_Recent_Images_From_Medical_Finder->addAction(recentFileAction);
	}

	updateRecentActionList();
}

void MainWindow::adjustForCurrentFile(const QString & filePath)
{
	QStringList recentFilePaths = settings->value("recentFiles").toStringList();

	recentFilePaths.removeAll(filePath);
	recentFilePaths.prepend(filePath);
	while (recentFilePaths.size() > MAX_RECENT_IMAGE)
		recentFilePaths.removeLast();
	settings->setValue("recentFiles", recentFilePaths);

	// see note
	updateRecentActionList();
}

void MainWindow::updateRecentActionList()
{
	QStringList recentFilePaths =
		settings->value("recentFiles").toStringList();

	int itEnd = 0;
	if (recentFilePaths.size() <= MAX_RECENT_IMAGE)
		itEnd = recentFilePaths.size();
	else
		itEnd = MAX_RECENT_IMAGE;

	for (int i = 0; i < itEnd; i++)
	{
		recentFileActionList.at(i)->setText(recentFilePaths.at(i));
		recentFileActionList.at(i)->setData(recentFilePaths.at(i));
		recentFileActionList.at(i)->setVisible(true);
	}

	for (int i = itEnd; i < MAX_RECENT_IMAGE; i++)
		recentFileActionList.at(i)->setVisible(false);
}


