#include "PoseIndicatorViewer.h"
//Qt
#include <QVTKInteractor.h>

//VTK
#include <vtkMatrix4x4.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkInteractorStyle.h>

#include <vtkImageActor.h>
#include <vtkActor2D.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKWidget2.h>

//me
#include "PositionIndicator.h"
#include "DepthIndicatorRepresentation.h"
#include "DepthIndicatorWidget.h"
#include "ui_PoseIndicatorViewer.h"

PoseIndicatorViewer::PoseIndicatorViewer(QWidget * parent)
	: PoseIndicatorViewer(50, 5, 5, 5.5, parent)
{
}


PoseIndicatorViewer::PoseIndicatorViewer(double depth, double remindDepth, double warningDepth, double nailLength, QWidget* parent)
	: m_depth(depth), m_remindingDepth(remindDepth), m_warningDepth(warningDepth), m_nailLength(nailLength), QWidget(parent)
{
	this->m_ui = new Ui::PoseIndicatorViewer;
	this->m_ui->setupUi(this);
	this->m_PIViewer = new PositionIndicator;
	this->m_Representation = DepthIndicatorRepresentation::New();
	this->m_DIWidget = DepthIndicatorWidget::New();	

	m_PIViewer->InitialPosition(m_targetPosition, m_targetOrientation);
	m_PIViewer->UpdatePosition(m_targetPosition, m_targetOrientation);

	m_renderer = vtkSmartPointer<vtkRenderer>::New();
	m_renderer->AddActor(this->m_PIViewer->m_backgroundActor);
	m_renderer->AddActor(this->m_PIViewer->m_drillActor);
	m_renderer->AddActor(this->m_PIViewer->m_crossActor);
	m_renderer->AddActor(this->m_PIViewer->m_textActor);
	m_renderer->SetBackground(0, 0, 0);

	//m_renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	m_renderWindow = this->m_ui->QVTKWidget->GetRenderWindow();
	m_renderWindow->AddRenderer(m_renderer);
	m_renderWindow->SetSize(1600, 800);

	//m_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	m_interactor = this->m_ui->QVTKWidget->GetInteractor();
	m_interactor->SetRenderWindow(m_renderWindow);

	//vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	m_interactorStyle = vtkSmartPointer<vtkInteractorStyle>::New();
	m_interactor->SetInteractorStyle(m_interactorStyle);

	this->m_DIWidget->SetInteractor(m_interactor);
	this->m_DIWidget->SetRepresentation(this->m_Representation);
	//this->m_DIWidget->positioncoordinate
	this->m_Representation->SetFinalDepth(m_depth);
	this->m_Representation->SetCurrentDepth(m_depth);
	this->m_Representation->SetRemindingDepth(m_remindingDepth);
	this->m_Representation->SetWarningDepth(m_warningDepth);

	this->m_Representation->SetPosition(0.1,0.1);
	//this->m_DIWidget->SelectableOn();
	//this->m_DIWidget->ResizableOff();
	this->m_DIWidget->On();
	
	//this->show();
}

PoseIndicatorViewer::~PoseIndicatorViewer()
{
	this->m_Representation->Delete();
	this->m_DIWidget->Delete();
	delete this->m_PIViewer;
	delete this->m_ui;
}

void PoseIndicatorViewer::setTargetPose(vtkMatrix4x4 * mat)
{
	m_targetPosition[0] = mat->GetElement(0, 3);
	m_targetPosition[1] = mat->GetElement(1, 3);
	m_targetPosition[2] = mat->GetElement(2, 3);
	m_targetOrientation[0] = mat->GetElement(0, 2);
	m_targetOrientation[1] = mat->GetElement(1, 2);
	m_targetOrientation[2] = mat->GetElement(2, 2);
	/////////////////////////////////////////////////
	m_targetPosition[0] -= m_nailLength * m_targetOrientation[0];
	m_targetPosition[1] -= m_nailLength * m_targetOrientation[1];
	m_targetPosition[2] -= m_nailLength * m_targetOrientation[2];
	//////////////////////////////////////////////////
	m_PIViewer->InitialPosition(m_targetPosition, m_targetOrientation);
	m_PIViewer->UpdatePosition(m_targetPosition, m_targetOrientation);
}

void PoseIndicatorViewer::setTrackerPosition(vtkMatrix4x4 * mat)
{
	double cPos[3];
	double cOri[3];
	cPos[0] = mat->GetElement(0, 3);
	cPos[1] = mat->GetElement(1, 3);
	cPos[2] = mat->GetElement(2, 3);
	cOri[0] = mat->GetElement(0, 2);
	cOri[1] = mat->GetElement(1, 2);
	cOri[2] = mat->GetElement(2, 2);

	double diffVec[3];
	//cout << "In timer Result!!!!!!!!!!!\n";
	this->m_PIViewer->UpdatePosition(cPos, cOri);
	vtkMath::Subtract(cPos, m_targetPosition, diffVec);
	vtkMath::ProjectVector(diffVec, m_targetOrientation, diffVec);
	//double depth = vtkMath::Distance2BetweenPoints(tPos, diffVec);
	//depth = sqrt(depth);
	double depth = vtkMath::Norm(diffVec);
	if (vtkMath::AngleBetweenVectors(diffVec, m_targetOrientation) > (3.14 / 2))
	{
		depth = -1 * depth;
	}
	
	depth = m_depth - depth;
	if (depth < 0)
	{
		depth = 0;
	}
	this->m_Representation->SetCurrentDepth(depth);
	this->vtkRender();
}

void PoseIndicatorViewer::vtkRender()
{
	this->m_renderer->ResetCamera();
	this->m_renderer->ResetCameraClippingRange();
	this->m_renderWindow->Render();
}


void PoseIndicatorViewer::setDepth(double depth)
{
	this->m_depth = depth;
	this->m_Representation->SetFinalDepth(m_depth);
}

void PoseIndicatorViewer::setRemindingDepth(double depth)
{
	this->m_remindingDepth = depth;
	this->m_Representation->SetRemindingDepth(m_remindingDepth);
}

void PoseIndicatorViewer::setWarningDepth(double depth)
{
	this->m_warningDepth = depth;
	this->m_Representation->SetWarningDepth(m_warningDepth);
}

void PoseIndicatorViewer::setNailLength(double length)
{
	this->m_nailLength = length * 0.5;
}

//void PoseIndicatorViewer::thisRender()
//{
//
//	
//	m_renderer = vtkSmartPointer<vtkRenderer>::New();
//	m_renderer->AddActor(this->m_PIViewer->m_backgroundActor);
//	m_renderer->AddActor(this->m_PIViewer->m_drillActor);
//	m_renderer->AddActor(this->m_PIViewer->m_crossActor);
//	m_renderer->AddActor(this->m_PIViewer->m_textActor);
//	
//	m_renderer->SetBackground(0,0,0);
//
//	m_renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
//	m_renderWindow->SetSize(800, 800);
//
//	m_renderWindow->AddRenderer(m_renderer);
//
//	m_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
//	m_interactor->SetRenderWindow(m_renderWindow);
//
//	//vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
//	m_interactorStyle = vtkSmartPointer<vtkInteractorStyleImage>::New();
//	m_interactor->SetInteractorStyle(m_interactorStyle);
//	m_renderWindow->Render();
//	m_interactor->Initialize();
//}
