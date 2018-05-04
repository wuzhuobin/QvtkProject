#include <vtkActor.h>
#include <vtkDiskSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkLineSource.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkDistanceRepresentation2D.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkAxisActor2D.h>
#include <vtkMath.h>
#include <vtkVectorText.h>
#include <vtkActor2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPNGReader.h>
#include <vtkImageData.h>


//renderer
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyleTrackballActor.h>

#include <iostream>
#include <cmath>


// me 
#include "PositionIndicator.h"
#include "vtkQRCFileReader.h"

using namespace std;

const struct PositionIndicatorResources
{
	PositionIndicatorResources() {
		Q_INIT_RESOURCE(PoseIndicator);
	}
} RESOURCES;

PositionIndicator::PositionIndicator(QObject* parent)
	: QObject(parent)
{
	m_drillActor = vtkSmartPointer<vtkImageActor>::New();
	vtkSmartPointer<vtkQRCFileReader<vtkPNGReader>> drillReader =
		vtkSmartPointer<vtkQRCFileReader<vtkPNGReader>>::New();
	drillReader->SetFileName(":/PoseIndicator/drill.png");
	drillReader->Update();
	m_drillActor->GetMapper()->SetInputConnection(drillReader->GetReader()->GetOutputPort());
	m_drillActor->SetPosition(m_drillOffset);

	m_crossActor = vtkSmartPointer<vtkImageActor>::New();
	vtkSmartPointer<vtkQRCFileReader<vtkPNGReader>> crossReader =
		vtkSmartPointer<vtkQRCFileReader<vtkPNGReader>>::New();
	crossReader->SetFileName(":/PoseIndicator/cross.png");
	crossReader->Update();
	m_crossActor->GetMapper()->SetInputConnection(crossReader->GetReader()->GetOutputPort());
	m_crossActor->SetPosition(m_crossOffset);

	m_backgroundActor = vtkSmartPointer<vtkImageActor>::New();
	vtkSmartPointer<vtkQRCFileReader<vtkPNGReader>> backgroundReader =
		vtkSmartPointer<vtkQRCFileReader<vtkPNGReader>>::New();
	backgroundReader->SetFileName(":/PoseIndicator/back.png");
	backgroundReader->Update();
	m_backgroundActor->GetMapper()->SetInputConnection(backgroundReader->GetReader()->GetOutputPort());
	//m_backgroundActor->SetPosition(50, 0, 0);

	m_textActor = vtkSmartPointer<vtkActor2D>::New();
	m_text = vtkSmartPointer<vtkVectorText>::New();
	vtkSmartPointer<vtkPolyDataMapper2D> textMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
	vtkSmartPointer<vtkTransform> textScaleTransformer = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkTransformPolyDataFilter> textScaleTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	textScaleTransformFilter->SetTransform(textScaleTransformer);
	textScaleTransformFilter->SetInputConnection(m_text->GetOutputPort());
	//textScaleTransformer->Scale(15, 15, 1);
	textScaleTransformer->Scale(10, 10, 1);

	textMapper->SetInputConnection(textScaleTransformFilter->GetOutputPort());
	m_textActor->SetMapper(textMapper);

	m_textActor->SetPosition(30, 30);
	m_textActor->GetProperty()->SetColor(0.8, 0.8, 0.8);
	
	connect(this, &PositionIndicator::positionChanged, this, &PositionIndicator::setPositionChanged);
}

PositionIndicator::~PositionIndicator()
{

}

void PositionIndicator::SetTargetPosition(double *pos)
{
	m_targetPosition[0] = pos[0] * m_scale;
	m_targetPosition[1] = pos[1] * m_scale;
	m_targetPosition[2] = pos[2] * m_scale;
}

void PositionIndicator::SetCurrentPosition(double *pos)
{
	m_currentPosition[0] = pos[0] * m_scale;
	m_currentPosition[1] = pos[1] * m_scale;
	m_currentPosition[2] = pos[2] * m_scale;
}

void PositionIndicator::SetTargetOrientation(double *pos)
{
	m_targetOrientation[0] = pos[0];
	m_targetOrientation[1] = pos[1];
	m_targetOrientation[2] = pos[2];
	vtkMath::Normalize(m_targetOrientation);
}

void PositionIndicator::SetCurrentOrientation(double *pos)
{
	m_currentOrientation[0] = pos[0];
	m_currentOrientation[1] = pos[1];
	m_currentOrientation[2] = pos[2];
	vtkMath::Normalize(m_currentOrientation);
}

void PositionIndicator::InitialPosition(double* targetPos, double* targetOri)
{
	m_targetPosition[0] = targetPos[0] * m_scale;
	m_targetPosition[1] = targetPos[1] * m_scale;
	m_targetPosition[2] = targetPos[2] * m_scale;
	m_targetOrientation[0] = targetOri[0];
	m_targetOrientation[1] = targetOri[1];
	m_targetOrientation[2] = targetOri[2];
	vtkMath::Normalize(m_targetOrientation);
}

void PositionIndicator::UpdatePosition(double* currentPos, double* currentOri)
{
	m_currentPosition[0] = currentPos[0] * m_scale;
	m_currentPosition[1] = currentPos[1] * m_scale;
	m_currentPosition[2] = currentPos[2] * m_scale;
	m_currentOrientation[0] = currentOri[0];
	m_currentOrientation[1] = currentOri[1];
	m_currentOrientation[2] = currentOri[2];
	vtkMath::Normalize(m_currentOrientation);

	//cross
	double crossPosition[3];
	CalculatePosition(m_currentPosition, crossPosition);
	double finalCrossPosition[3];

	vtkMath::Add(m_crossOffset, crossPosition, finalCrossPosition);
	m_crossActor->SetPosition(finalCrossPosition);
	m_crossActor->Modified();
	m_crossActor->Update();

	//drill
	double drillPosition[3];
	//double drillTop
	double drillTopPosition[3] = {
		m_currentPosition[0] - m_currentOrientation[0] * m_drillDistance,
		m_currentPosition[1] - m_currentOrientation[1] * m_drillDistance,
		m_currentPosition[2] - m_currentOrientation[2] * m_drillDistance
	};
	CalculatePosition(drillTopPosition, drillPosition);

	double finalDrillPosition[3];

	vtkMath::Add(m_drillOffset, drillPosition, finalDrillPosition);
	m_drillActor->SetPosition(finalDrillPosition);
	//cout << "Drill Position: " << finalDrillPosition[0] << " " << finalDrillPosition[1] << " " << finalDrillPosition[2] << endl;
	m_drillActor->Modified();
	m_drillActor->Update();

	//Text
	double dist = sqrt(vtkMath::Distance2BetweenPoints(m_targetPosition, m_currentPosition))/m_scale;
	double angle = vtkMath::AngleBetweenVectors(m_currentOrientation, m_targetOrientation) * 180 / vtkMath::Pi();

	sprintf(m_titleText, "Distance: %.1fmm    Angle: %.1f degree", dist, angle);
	m_text->SetText(m_titleText);
	m_textActor->Modified();
	m_text->Update();
}

void PositionIndicator::CalculatePosition(double * input, double *output)
{
	//step1 将目眮E闫揭浦猎鉕，将drill平移至点ptD
	double ptD[3] = { input[0] - m_targetPosition[0], input[1] - m_targetPosition[1], input[2] - m_targetPosition[2] };

	//step2 求法向量vecN
	double vecN[3] = { m_targetOrientation[0],m_targetOrientation[1],m_targetOrientation[2] };
	//vtkMath::Normalize(vecN);

	//step3 求投影点ptA
	double paraT = -vecN[0] * ptD[0] - vecN[1] * ptD[1] - vecN[2] * ptD[2];
	double ptA[3] = { ptD[0] + vecN[0] * paraT, ptD[1] + vecN[1] * paraT, ptD[2] + vecN[2] * paraT };

	//step4 求vecN觼E0,0,-1)夹角theta
	double vecOri[3] = { 0,0,-1 };
	double theta = vtkMath::AngleBetweenVectors(vecN, vecOri) * 180 / vtkMath::Pi();

	//step5 计算旋转轴vecP
	double vecP[3] = { vecN[1], -vecN[0], 0 };

	//step6 点ptA沿着vecP旋转theta角，得到ptF，显示到屏幕
	vtkSmartPointer<vtkTransform> rotateTF = vtkSmartPointer<vtkTransform>::New();
	rotateTF->RotateWXYZ(theta, vecP);
	double ptF[3];
	rotateTF->TransformPoint(ptA, ptF);

	//showArray(ptF, "PTF1");


	//rotateTF = vtkSmartPointer<vtkTransform>::New();
	//rotateTF->RotateWXYZ(-theta, vecP);
	//rotateTF->TransformPoint(ptA, ptF);
	//rotateTF->Update();
	//showArray(ptF, "PTF2");

	//showArray(ptF, "PTF");

	//判断选转方向是否正确
	if (abs(ptF[2]) > 0.01)
	{
		rotateTF = vtkSmartPointer<vtkTransform>::New();
		rotateTF->RotateWXYZ(-theta, vecP);
		rotateTF->TransformPoint(ptA, ptF);
	}


	rotateTF->Update();

	//showArray(ptD, "PTD");
	//showArray(ptA, "PTA");
	//showArray(ptF, "PTF");
	//showArray(vecN, "VECN");
	//showArray(vecP, "VECP");
	//cout << "theta=" << theta << endl;
	for (int i = 0; i < 3; i++)
	{
		output[i] = ptF[i];
	}
}


//void PositionIndicator::Render()
//{
//	//m_renderer = vtkSmartPointer<vtkRenderer>::New();
//	//m_renderer->AddActor(m_backgroundActor);
//	//m_renderer->AddActor(m_drillActor);
//	//m_renderer->AddActor(m_crossActor);
//	//m_renderer->AddActor(m_textActor);
//	//
//	//m_renderer->SetBackground(0,0,0);
//
//	//m_renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
//	//m_renderWindow->SetSize(800, 800);
//
//	//m_renderWindow->AddRenderer(m_renderer);
//
//	//m_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
//	//m_interactor->SetRenderWindow(m_renderWindow);
//
//	////vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
//	//m_style = vtkSmartPointer<vtkInteractorStyleImage>::New();
//	//m_interactor->SetInteractorStyle(m_style);
//	//m_renderWindow->Render();
//	//m_interactor->Initialize();
//}

void PositionIndicator::setPositionChanged(double* pos)
{


}

inline void PositionIndicator::showArray(double * array, char * name)
{
	cout << name << ": " << array[0] << "\t" << array[1] << "\t" << array[2] << endl;
}

