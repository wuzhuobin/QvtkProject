//me 
#include "Hub.h"
#include "QvtkScene.h"
#include "QvtkPolyData.h"
#include "QvtkImage.h"
#include "QvtkImageLabel2.h"
#include "QvtkImageLabel.h"
#include "QvtkVolume.h"
#include "QvtkPolyDataActor.h"
#include "QvtkImageSlice.h"
#include "QvtkImageSliceLabel.h"
#include "QvtkPolyDataActor2D.h"
#include "QvtkAnnotationPolyData.h"
#include "QvtkPolyDataSource.h"
#include "QvtkNeuralTube.h"
#include "itkQDebugWindow.h"
#include "vtkQDebugWindow.h"
// qt
#include <QApplication>
#include <QTranslator>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

namespace Q {
	namespace vtk {
		static const QString LOGGING_DATE_TIME = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
		struct SceneConfiguration
		{
			SceneConfiguration()
			{
				if (!QDir("LOG").exists()) {
					QDir(".").mkdir("LOG");
				}
				qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
					QByteArray localMsg = msg.toLocal8Bit();
					QString strMsg = QDateTime::currentDateTime().toString("yyyy/MM/dd    hh:mm:ss:zzz    ");
					switch (type) {
					case QtDebugMsg:
						strMsg += "Debug: \n";
						break;
					case QtInfoMsg:
						strMsg += "Info: \n";
						break;
					case QtWarningMsg:
						strMsg += "Warning: \n";
						break;
					case QtCriticalMsg:
						strMsg += "Critical: \n";
						break;
					case QtFatalMsg:
						strMsg += "Fatal: \n";
						break;
					}

					QString strMessage = strMsg + QString("\n%1 (%2:%3, %4)\n")
						.arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
					cerr << strMessage.toStdString() << "\n";
					QFile file("LOG/LOG_" + LOGGING_DATE_TIME + ".txt");
					file.open(QIODevice::ReadWrite | QIODevice::Append);
					QTextStream stream(&file);
					stream << strMessage << "\r\n";
					file.flush();
					file.close();
				});

				itk::OutputWindow::SetInstance(itk::QDebugWindow::New());
				vtkOutputWindow::SetInstance(vtkQDebugWindow::New());

				Scene* scene = new Scene();
				scene->registerData(new AnnotationPolyData);
				scene->registerData(new Image);
				scene->registerData(new ImageLabel);
				scene->registerData(new ImageLabel2);
				scene->registerData(new ImageSlice);
				scene->registerData(new ImageSliceLabel);
				scene->registerData(new PolyData);
				scene->registerData(new PolyDataActor);
				scene->registerData(new PolyDataActor2D);
				scene->registerData(new PolyDataSource);
				scene->registerData(new Volume);
			}
		};


	}
}

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#pragma comment(linker, "/SUBSYSTEM:console /ENTRY:mainCRTStartup")

int main(int argc, char* argv[])
{
	//using namespace QsLogging;

	QApplication app(argc, argv);
	Q::vtk::SceneConfiguration();
	//app.setAttribute(Qt::AA_EnableHighDpiScaling);

	QTranslator qtTranslator;
	qtTranslator.load("lang_" + QLocale::system().name());
	app.installTranslator(&qtTranslator);

	//QFile styleFile(":/HyperBlue/hyperblue/styles.qss");
	//if (styleFile.open(QIODevice::ReadOnly)) {
	//	app.setStyleSheet(styleFile.readAll());
	//}
	Hub hub;
	QCoreApplication::processEvents();
	////QPixmap pixmap(":/HyperBlue/hyperblue/icon/product-splashScreen.png");
	//QPixmap pixmap(":/HyperBlue/hyperblue/icon/Orthopedics-splashscreen2.png");
	//QSplashScreen splash(pixmap);
	//splash.show();
	//app.processEvents();
	
	//splash.finish(NULL);

	return app.exec();
}

