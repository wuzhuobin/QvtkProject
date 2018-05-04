#ifndef __SCB_HUB_H__
#define __SCB_HUB_H__

// me
#include "MainWindow.h"
#include "QvtkStylesSwitchOrthogonalViewer.h"
#include "QvtkWidgetCollection.h"
//qt
#include <QObject>

class Hub final: public QObject
{
	Q_OBJECT;
public:
	static const int NUM_OF_ORTHOGONAL_VIEWER = 4;
	static const int NUM_OF_2D_VIEWERS = 3;
	static Hub* self;
  	explicit Hub(QObject* parent = nullptr);
    virtual ~Hub() override;

	MainWindow* mainWindow;
	Q::vtk::StylesSwitchOrthogonalViewer* styles[4];
	Q::vtk::WidgetCollection* widgets[4];

public slots:
	//void 
	void slotInitializationImages(QStringList imagePaths);
	
	void slotInitializationImport();
	void slotInitializationPolyDataSourceWidget();
	void slotInitializationBiopsyWidget();
	void slotInitializationMainWindow();

	void slotCommit(QString path);
	void slotImport(QString path, bool clean);
	void slotImportString(QString string);
	void slotClean();
	void slotExit();


	// interactorstyle
	void slotInteractorStyleTesting(bool flag);
	void slotInteractorStyleWindowLevel(bool flag);
	void slotInteractorStyleNavigation(bool flag);
	void slotInteractorStyleDentalPlanning(bool flag);
	void slotInteractorStyleSeedPlacer(bool flag);

	// widget
	void slotAddDistanceWidget();
	void slotRemoveDistanceWidget();
	void slotPlanarSeedWidget(bool flag);
	void slotAddImplantWidget();
	void slotRemoveImplantWidget();
	void slotAddPolyDataSouceWidget();
	void slotRemovePolyDataSourceWidget();
	void slotAddBiopsyWidget();
	void slotRemoveBiopsyWidget();


	// testing
	void slotTestingAction();



};

#endif // !__SCB_HUB_H__