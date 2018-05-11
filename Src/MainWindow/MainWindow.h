#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

// me
#include "QvtkOrthogonalDockViewers.h"
#include "QvtkSceneWidget.h"
#include "QvtkOrthogonalViewer.h"
#include "ui_MainWindow.h"
#include "ctkDicomAppWidget2.h"

// qt
#include <QMainWindow>
#include <QActionGroup>
namespace Ui {
	class MainWindow;
}
class QSettings;

class  MainWindow final:
	public QMainWindow,
	public Ui::MainWindow
{
	Q_OBJECT;
public:
	const static int MAX_RECENT_IMAGE = 10;

	explicit MainWindow(int numOfViewers, QWidget* parent = nullptr);
	virtual ~MainWindow() override;

	Q::vtk::OrthogonalViewer* getViewer(int i);
public slots:
	// menu file
	void slotImportImages(QString path = QString());
	void slotImportImagesFromDatabase();
	void slotRecentImages();
	void slotImportProject(QString path = QString(), bool clean = true);
	void slotImportProjectString(QString xml = QString());
	void slotCommitProject(QString path = QString());
	

signals:
	void signalImportImages(QStringList paths);
	void signalImportedProject(QString path, bool clean);
	void signalImportedProjectString(QString string);
	void signalCommitedProject(QString path);

protected:
	virtual void closeEvent(QCloseEvent* event) override;

private:
	//Recent File
	void createRecentImageActions();
	void adjustForCurrentFile(const QString& filePath);
	void updateRecentActionList();

	QList<QAction*> recentFileActionList;
	QSettings* settings;
	static const QString PASSWORD;
	QActionGroup stylesGroup;
	Q::vtk::SceneWidget sceneWidget;
	Q::vtk::OrthogonalDockViewers viewers;
	ctkDICOMAppWidget2 databaseWidget;
};


#endif // !__MAIN_WINDOW_H__
