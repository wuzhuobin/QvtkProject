#include <QApplication>
#include "TrackerVolumeViewer.h"

int main( int argc, char** argv )
{
  // QT Stuff
  QApplication app( argc, argv );

  TrackerVolumeViewer* trackerVolumeViewer = new TrackerVolumeViewer;
  trackerVolumeViewer->SetOrientation(2);
  trackerVolumeViewer->SetTracker(TrackerVolumeViewer::HX40);
  trackerVolumeViewer->Initialize();
  //trackerVolumeViewer->SetTrackingTool(0, "E:/TestProgram/TrackerVolumeViewer/resources/tool/sphere.stl");
  trackerVolumeViewer->ResetCamera();
  trackerVolumeViewer->Render();

  return app.exec();
}
