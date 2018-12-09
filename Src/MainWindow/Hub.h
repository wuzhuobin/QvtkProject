#ifndef __SCB_HUB_H__
#define __SCB_HUB_H__
#pragma once
// me
#include "mainwindow_export.h"
#include "MainWindow.h"
#include "QvtkStylesSwitchOrthogonalViewer.h"
#include "QvtkWidgetCollection.h"
#include "QvtkStackedFilters.h"
//qt
#include <QObject>
class MAINWINDOW_EXPORT Hub final: public QObject
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
  Q::vtk::StackedFilter *filters;

  Q_DISABLE_COPY(Hub);
public Q_SLOTS:
  //void 
  void slotInitializationImages(QStringList imagePaths);
  void slotInitializationLabel(QString path);
  void slotInitializationImagesWithLUT(QStringList  path);
  void slotInitializationModel(QString path);
  void slotInitializationImport();
  void slotInitialization();
  void slotInitializationPolyDataSourceWidget();
  void slotInitializationPolyDataSourceNormal();
  void slotInitializationBiopsyWidget();
  void slotInitializationPlanarSeedWidges();

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
  void slotAddPolyDataSouceNormal();
  void slotRemovePolyDataSourceNormal();
  void slotAddBiopsyWidget();
  void slotRemoveBiopsyWidget();
  // filter
  void slotBrainExtractionFilter();
  // testing
  void slotTestingAction();
};

#endif // !__SCB_HUB_H__
