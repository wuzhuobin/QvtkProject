//#include "PositionIndicatorViewer.h"
#include <QtWidgets>
#include <QtCore>
#include <QtGui>
#include <qobject.h>
//#include "SCBMicrontracker.h"
//#include "DepthIndicatorRepresentation.h"
//#include "DepthIndicatorWidget.h"

#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkCommand.h>
#include <windows.h>
#include <iostream>

//#include <vtkWin32OpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkActor2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>

#include "PoseIndicatorViewer.h"

//#include "QTimer";

double targetPosition[3] = { 0,0,0 };
double currentPosition[3] = { 0.3, 0.5, 0.2 };

double targetOrientation[3] = { 2,0.5,-1 };
double currentOrientation[3] = { 2.1,0.8,-1.1 };
double tPos[3];
double tOri[3];
double gDepth = 3;

class vtkTimerCallback : public vtkCommand
{
public:
	static vtkTimerCallback *New()
	{
		vtkTimerCallback *cb = new vtkTimerCallback;
		return cb;
	}

	virtual void Execute(vtkObject *vtkNotUsed(caller),
		                 unsigned long vtkNotUsed(eventId), 
		                 void* vtkNotUsed(callData))
	{
		//cout << "New Position: " << timerViewer->m_currentPosition[0] << " " << timerViewer->m_currentPosition[1] << " " << timerViewer->m_currentPosition[2] << "\n";
		vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
		int result = 0;   //timerTracker->getRawTransformMatrix(1, mat);
		double cPos[3];
		double cOri[3];
		/*cPos[0] = mat->GetElement(0, 3);
		cPos[1] = mat->GetElement(1, 3);
		cPos[2] = mat->GetElement(2, 3);
		cOri[0] = mat->GetElement(0, 2);
		cOri[1] = mat->GetElement(1, 2);
		cOri[2] = mat->GetElement(2, 2);*/
		//int result = 0;
		if (result == 0)
		{
			double diffVec[3];
			cout << "In timer Result!!!!!!!!!!!\n";
			timerViewer->slotGetTrackerPosition(mat);
			//timerViewer->UpdatePosition(cPos, cOri);
			//vtkMath::Subtract(cPos, tPos, diffVec);
			//vtkMath::ProjectVector(diffVec, tOri, diffVec);
			////double depth = vtkMath::Distance2BetweenPoints(tPos, diffVec);
			////depth = sqrt(depth);
			//double depth = vtkMath::Norm(diffVec);
			//depth = 50 - depth;
			//currentPosition[0] += 0.04; currentPosition[1] += 0.04; gDepth += 0.002;
			//timerViewer->UpdatePosition(currentPosition, currentOrientation);
			//timerRep->SetCurrentDepth(depth);
			//timerViewer->m_renderer->Render();
			//timerViewer->m_interactor->Render();
		}
		cout << "In timer: " << result << "\n";
	}

	//PositionIndicator * timerViewer;
	//DepthIndicatorRepresentation* timerRep;
	PoseIndicatorViewer* timerViewer;
	//SCBMicronTracker* timerTracker;
};

double step = 0;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//SCBMicronTracker* tracker = new SCBMicronTracker();
		
	PoseIndicatorViewer* viewer = new PoseIndicatorViewer();
	viewer->show();
	QTimer timer1;
	timer1.setInterval(1000);
	timer1.start();
	QObject::connect(&timer1, &QTimer::timeout, [=]() {
		vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
		mat->Identity();
		step += 0.3;
		mat->SetElement(0, 3, step);
		mat->SetElement(1, 3, step);
		mat->SetElement(2, 3, step);
		mat->Modified();
		viewer->slotGetTrackerPosition(mat);
		//tracker->startRecording("");
	});
	////////////////////////////////////////////////////////////////////
	//int result = tracker->setupCameras();
	//Sleep(1000);
	//cout << "Connection? " << tracker->isConnected() << " Error Code: " << result << endl;
	//cout << "Thermal status? " << tracker->getCameraThermalStatus() << endl;
	//MTStatus status;
	//status = tracker->getCurrentStatus();
	//cout << "Minutes from powerup? " << status.powerupTime << endl;
	//tracker->getMarkerTemplates();
	//tracker->setToolID(0, "TTblock");
	//tracker->setToolID(1, "BoneNeedle");
	//system("pause");
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
	//tracker->getRawTransformMatrix(0, mat);
	//viewer->setInitialPose(mat);					//Set initial Pose, give a vtkMatrix4x4


	//vtkSmartPointer<vtkRenderer> Renderer = vtkSmartPointer<vtkRenderer>::New();
	//Renderer->AddActor(viewer->m_PIViewer->m_backgroundActor);			// Add 4 Actor to renderer
	//Renderer->AddActor(viewer->m_PIViewer->m_drillActor);
	//Renderer->AddActor(viewer->m_PIViewer->m_crossActor);
	//Renderer->AddActor(viewer->m_PIViewer->m_textActor);

	//Renderer->SetBackground(0, 0, 0);

	//vtkSmartPointer<vtkRenderWindow> RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	//RenderWindow->SetSize(800, 800);

	//RenderWindow->AddRenderer(Renderer);

	//vtkSmartPointer<vtkRenderWindowInteractor> WindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	//WindowInteractor->SetRenderWindow(RenderWindow);

	////vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	//vtkSmartPointer<vtkInteractorStyleImage> InteractorStyleStyle = vtkSmartPointer<vtkInteractorStyleImage>::New();
	//WindowInteractor->SetInteractorStyle(InteractorStyleStyle);
	//RenderWindow->Render();
	//WindowInteractor->Initialize();

	//viewer->setInterator(WindowInteractor);



	//Create my widget
	//vtkSmartPointer<DepthIndicatorRepresentation> rep = vtkSmartPointer<DepthIndicatorRepresentation>::New();
	//vtkSmartPointer<DepthIndicatorWidget> widget = vtkSmartPointer<DepthIndicatorWidget>::New();
	//widget->SetInteractor(m_interactor);
	//widget->SetRepresentation(rep);

	//rep->SetFinalDepth(50);
	//rep->SetCurrentDepth(10.5);
	//rep->SetRemindingDepth(5);
	//rep->SetWarningDepth(5);

	//m_renderWindow->Render();
	//widget->On();

	//////Timer for simulate the signal and slots
	//vtkSmartPointer<vtkTimerCallback> timerCb = vtkSmartPointer<vtkTimerCallback>::New();
	//timerCb->timerViewer = viewer;
	//timerCb->timerTracker = tracker;
	//WindowInteractor->AddObserver(vtkCommand::TimerEvent, timerCb);
	//WindowInteractor->CreateRepeatingTimer(10);
	//

	//WindowInteractor->Start();
	//QTimer timer;
	//timer.setInterval(100);
	//timer.start();
	//QObject::connect(&timer, &QTimer::timeout, [=]() {

	//});

	return a.exec();


	////offset
	//double drillOffset[3] = { 257,23,0 };
	//double crossOffset[3] = { 258,259,0 };

	////get&set
	//double targetPosition[3] = { 0,0,0 };
	//double currentPosition[3] = { 20,10,0 };

	//double targetOrientation[3] = { 0,1,-1 };
	//double currentOrientation[3] = { 0,0,-1 };

	////distance
	//double drillDistance = 20;


	////drill image
	//vtkSmartPointer<vtkImageActor> drillActor = vtkSmartPointer<vtkImageActor>::New();
	//vtkSmartPointer<vtkPNGReader> drillReader = vtkSmartPointer<vtkPNGReader>::New();

	//drillReader->SetFileName("../drill.png");
	//drillReader->Update();

	//drillActor->GetMapper()->SetInputConnection(drillReader->GetOutputPort());
	//drillActor->SetPosition(drillOffset);


	////background
	//vtkSmartPointer<vtkImageActor> backgroundActor = vtkSmartPointer<vtkImageActor>::New();
	//vtkSmartPointer<vtkPNGReader> backgroundReader = vtkSmartPointer<vtkPNGReader>::New();

	//backgroundReader->SetFileName("../back.png");
	//backgroundReader->Update();
	//backgroundActor->GetMapper()->SetInputConnection(backgroundReader->GetOutputPort());


	////cross
	//vtkSmartPointer<vtkImageActor> crossActor = vtkSmartPointer<vtkImageActor>::New();
	//vtkSmartPointer<vtkPNGReader> crossReader = vtkSmartPointer<vtkPNGReader>::New();

	//crossReader->SetFileName("../cross.png");
	//crossReader->Update();
	//crossActor->GetMapper()->SetInputConnection(crossReader->GetOutputPort());
	//crossActor->SetPosition(crossOffset);

	//vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	//renderer->AddActor(backgroundActor);
	//renderer->AddActor(drillActor);
	//renderer->AddActor(crossActor);

	////////////////////////////////////////////////////////////////////////////////////////////
	////step1 将目眮E闫揭浦猎鉕，将drill平移至点ptD
	//double ptD[3] = { currentPosition[0] - targetPosition[0], currentPosition[1] - targetPosition[1], currentPosition[2] - targetPosition[2] };

	////step2 求法向量vecN
	//double vecN[3] = { targetOrientation[0],targetOrientation[1],targetOrientation[2] };
	//vtkMath::Normalize(vecN);

	////step3 求投影点ptA
	//double paraT = -vecN[0]*ptD[0]-vecN[1]*ptD[1]-vecN[2]*ptD[2];
	//double ptA[3] = { ptD[0] + vecN[0] * paraT, ptD[1] + vecN[1] * paraT, ptD[2] + vecN[2] * paraT };

	////step4 求vecN觼E0,0,-1)夹角theta
	//double vecOri[3] = { 0,0,-1 };
	//double theta = vtkMath::AngleBetweenVectors(vecN, vecOri)*180/vtkMath::Pi();

	////step5 计算旋转轴vecP
	//double vecP[3] = { vecN[1], -vecN[0], 0 };



	////step6 点ptA沿着vecP旋转theta角，得到ptF，显示到屏幕
	//vtkSmartPointer<vtkTransform> rotateTF = vtkSmartPointer<vtkTransform>::New();
	//rotateTF->RotateWXYZ(theta, vecP);
	//double ptF[3];
	//rotateTF->TransformPoint(ptA, ptF);
	//rotateTF->Update();
	////判断选转方向是否正确
	//if (abs(ptF[3])>0.01)
	//{
	//	rotateTF = vtkSmartPointer<vtkTransform>::New();
	//	rotateTF->RotateWXYZ(-theta, vecP);
	//	rotateTF->TransformPoint(ptA, ptF);
	//	rotateTF->Update();
	//}
	//showArray(ptD, "PTD");
	//showArray(ptA, "PTA");
	//showArray(ptF, "PTF");
	//showArray(vecN, "VECN");
	//showArray(vecP, "VECP");
	//cout << "theta=" <<theta<< endl;

	//double crossPosition[3];
	//vtkMath::Add(crossOffset, currentPosition, crossPosition);
	//crossActor->SetPosition(crossPosition);
	//crossActor->Modified();

	////vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
	////camera->SetPosition(0, 0, 1000);
	////camera->SetParallelProjection(true);

	////renderer->SetActiveCamera(camera);


	////render
	//

	////camera->ParallelProjectionOn();


	////renderer->AddActor(centerActor);
	////renderer->AddActor(circleActor);
	////renderer->AddActor(drillActor);
	////renderer->AddActor(verticalLineActor);
	////renderer->AddActor(horizontalLineActor);
	////renderer->AddActor(imageActor);
	////renderer->AddActor(lineActor);


	//
	//renderer->SetBackground(0.5, 0.7, 1);




	//vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	//renderWindow->SetSize(800,800);
	//
	//renderWindow->AddRenderer(renderer);

	//vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	//interactor->SetRenderWindow(renderWindow);
	//
	////vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	//vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();
	//interactor->SetInteractorStyle(style);
	//renderWindow->Render();
	//interactor->Start();


}




////center
//vtkSmartPointer<vtkDiskSource> centerSource = vtkSmartPointer<vtkDiskSource>::New();
//centerSource->SetCircumferentialResolution(80);
//centerSource->SetRadialResolution(8);
//centerSource->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION);

//centerSource->SetInnerRadius(0);
//centerSource->SetOuterRadius(2);
//centerSource->Update();

//vtkSmartPointer<vtkPolyDataMapper> centerMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//centerMapper->SetInputConnection(centerSource->GetOutputPort());

//vtkSmartPointer<vtkActor> centerActor = vtkSmartPointer<vtkActor>::New();
//centerActor->SetMapper(centerMapper);
//centerActor->SetPosition(0, 0, 0);
//centerActor->GetProperty()->SetColor(1, 0, 0);


////circle
//vtkSmartPointer<vtkDiskSource> circleSource = vtkSmartPointer<vtkDiskSource>::New();
//circleSource->SetCircumferentialResolution(80);
//circleSource->SetRadialResolution(8);
//circleSource->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION);
//
//circleSource->SetInnerRadius(5);
//circleSource->SetOuterRadius(5.25);
//circleSource->Update();

//vtkSmartPointer<vtkPolyDataMapper> circleMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//circleMapper->SetInputConnection(circleSource->GetOutputPort());

//vtkSmartPointer<vtkActor> circleActor = vtkSmartPointer<vtkActor>::New();
//circleActor->SetMapper(circleMapper);
//circleActor->SetPosition(0,0,0);
//circleActor->GetProperty()->SetColor(1, 0, 0);

////crosshair
//vtkSmartPointer<vtkLineSource> verticalLineSource = vtkSmartPointer<vtkLineSource>::New();
//verticalLineSource->SetPoint1(0, -20, 0);
//verticalLineSource->SetPoint2(0, 20, 0);
//verticalLineSource->Update();
//vtkSmartPointer<vtkPolyDataMapper>verticalLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//verticalLineMapper->SetInputConnection(verticalLineSource->GetOutputPort());

//vtkSmartPointer<vtkActor>verticalLineActor = vtkSmartPointer<vtkActor>::New();
//verticalLineActor->SetMapper(verticalLineMapper);

//vtkSmartPointer<vtkLineSource> horizontalLineSource = vtkSmartPointer<vtkLineSource>::New();
//horizontalLineSource->SetPoint1(-20, 0, 0);
//horizontalLineSource->SetPoint2(20, 0, 0);
//horizontalLineSource->Update();
//vtkSmartPointer<vtkPolyDataMapper>horizontalLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//horizontalLineMapper->SetInputConnection(horizontalLineSource->GetOutputPort());

//vtkSmartPointer<vtkActor>horizontalLineActor = vtkSmartPointer<vtkActor>::New();
//horizontalLineActor->SetMapper(horizontalLineMapper);

////drill
//vtkSmartPointer<vtkDiskSource> drillSource = vtkSmartPointer<vtkDiskSource>::New();
//drillSource->SetCircumferentialResolution(80);
//drillSource->SetRadialResolution(8);
//drillSource->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION);

//drillSource->SetInnerRadius(0);
//drillSource->SetOuterRadius(1.6);
//drillSource->Update();

//vtkSmartPointer<vtkPolyDataMapper> drillMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//drillMapper->SetInputConnection(drillSource->GetOutputPort());

//vtkSmartPointer<vtkActor> drillActor = vtkSmartPointer<vtkActor>::New();
//drillActor->SetMapper(drillMapper);
//drillActor->SetPosition(0, 0, 0);
//drillActor->GetProperty()->SetColor(0.6, 0.4, 0);
//drillActor->GetProperty()->SetOpacity(0.8);
//drillActor->SetPosition(2, 0, 1);