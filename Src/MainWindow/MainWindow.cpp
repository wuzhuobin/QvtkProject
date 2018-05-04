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
		this->viewers.setViewer(i, new Q::vtk::PlanarViewer(&this->viewers));
	}
	this->viewers.setViewer(numOfViewers - 1, new Q::vtk::NonPlanarViewer(&this->viewers));
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
	connect(this->action_Import_Images, SIGNAL(triggered()),
		this, SLOT(slotImportImages()));
	connect(this->action_Commit_Project, SIGNAL(triggered()),
		this, SLOT(slotCommitProject()));
	connect(this->action_Import_Project, SIGNAL(triggered()),
		this, SLOT(slotImportProject()));
	connect(this->action_Import_Project_Without_Clean, &QAction::triggered,
		this, [this]() {this->slotImportProject(QString(), false);});
	connect(this->action_Import_Project_String, SIGNAL(triggered()),
		this, SLOT(slotImportProjectString()));

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

void MainWindow::slotImportImages(QString path)
{
	MedicalImageFinder rw(path, 4);
	rw.setImageModalityNames(0, "Image 0");
	rw.setImageModalityNames(1, "Image 1");
	rw.setImageModalityNames(2, "Image 2");
	rw.setImageModalityNames(3, "Image 3");

	if (QWizard::Accepted == rw.exec()) {

		QStringList imagePaths;
		for (int i = 0; i < rw.getNumberOfImages(); ++i) {
			if (rw.getFileNames(i).isEmpty()) {
				continue;
			}
			imagePaths << rw.getFileNames(i);
		}
		adjustForCurrentFile(rw.getDirectory());
		emit signalImportImages(imagePaths);
	}
}
void MainWindow::slotRecentImages()
{
	QCoreApplication::processEvents();

	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
	{
		slotImportImages(action->data().toString());
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
		connect(recentFileAction, SIGNAL(triggered()), this, SLOT(slotRecentImages()));

		recentFileActionList.append(recentFileAction);
		this->menu_Recent_Images->addAction(recentFileAction);
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


