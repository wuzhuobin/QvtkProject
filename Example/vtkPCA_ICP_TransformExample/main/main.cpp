// me
#include "vtkPCA_ICP_Transform.h"

// vtk
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkLandmarkTransform.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTransformPolyDataFilter.h>

const unsigned char LENGTH = 100;
int main(int argc, char* argv[])
{
	vtkSmartPointer<vtkPolyDataReader> reader1 =
		vtkSmartPointer<vtkPolyDataReader>::New();
	reader1->SetFileName("Dental(shift mid point to origin).vtk");
	reader1->Update();


	vtkSmartPointer<vtkPolyDataReader> reader2 =
		vtkSmartPointer<vtkPolyDataReader>::New();
	reader2->SetFileName("Reference with develop tool.vtk");
	reader2->Update();
	
	//vtkSmartPointer<vtkPolyDataReader> reader3 =
	//	vtkSmartPointer<vtkPolyDataReader>::New();
	//reader3->SetFileName("Develop tool only.stl");
	//reader3->Update();

	vtkSmartPointer<vtkPCA_ICP_Transform> icp =
		vtkSmartPointer<vtkPCA_ICP_Transform>::New();
	icp->GetLandmarkTransform()->SetModeToRigidBody();
	icp->SetTarget(reader1->GetOutput());
	icp->SetSource(reader2->GetOutput());
	icp->Update();


	cout << "RMS is " << icp->GetRMS() << '\n';
	
	//vtkSmartPointer<vtkTransformPolyDataFilter> transformPolyDataFilter =
	//	vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	//transformPolyDataFilter->SetInputConnection(reader3->GetOutputPort());
	//transformPolyDataFilter->SetTransform(icp);

	//vtkSmartPointer<vtkPolyDataWriter> writer =
	//	vtkSmartPointer<vtkPolyDataWriter>::New();
	////writer->SetInputData(icp->GetTransformPolyData());
	//writer->SetInputConnection(transformPolyDataFilter->GetOutputPort());
	//writer->SetFileName("icp.vtk");
	//writer->Write();

	cin.get();

	//vtkSmartPointer<vtkPolyDataMapper> polyDataMapper1 =
	//	vtkSmartPointer<vtkPolyDataMapper>::New();
	//polyDataMapper1->SetInputConnection(reader1->GetOutputPort());


	//vtkSmartPointer<vtkActor> actor1 =
	//	vtkSmartPointer<vtkActor>::New();
	//actor1->SetMapper(polyDataMapper1);
	//actor1->GetProperty()->SetColor(1, 0, 0);


	//vtkSmartPointer<vtkPolyDataMapper> polyDataMapper2 =
	//	vtkSmartPointer<vtkPolyDataMapper>::New();
	//polyDataMapper2->SetInputConnection(reader2->GetOutputPort());


	//vtkSmartPointer<vtkActor> actor2 =
	//	vtkSmartPointer<vtkActor>::New();
	//actor2->SetMapper(polyDataMapper2);
	//actor2->GetProperty()->SetColor(0, 1, 0);

	//vtkSmartPointer<vtkPolyDataMapper> polyDataMapper3 =
	//	vtkSmartPointer<vtkPolyDataMapper>::New();
	//polyDataMapper3->SetInputData(icp->GetTransformPolyData());
	////polyDataMapper3->SetInputConnection(reader3->GetOutputPort());


	//vtkSmartPointer<vtkActor> actor3 =
	//	vtkSmartPointer<vtkActor>::New();
	//actor3->SetMapper(polyDataMapper3);
	//actor3->GetProperty()->SetColor(0, 0, 1);

	//

	//vtkSmartPointer<vtkSphereSource> sphereSource1 =
	//	vtkSmartPointer<vtkSphereSource>::New();
	//sphereSource1->SetCenter(icp->GetTargetCenter());
	//sphereSource1->SetRadius(LENGTH * 0.1);


	//vtkSmartPointer<vtkPolyDataMapper> sphereMapper1 =
	//	vtkSmartPointer<vtkPolyDataMapper>::New();
	//sphereMapper1->SetInputConnection(sphereSource1->GetOutputPort());


	//vtkSmartPointer<vtkActor> sphereActor1 =
	//	vtkSmartPointer<vtkActor>::New();
	//sphereActor1->SetMapper(sphereMapper1);
	//sphereActor1->GetProperty()->SetColor(1, 0, 0);



	//vtkSmartPointer<vtkSphereSource> sphereSource2 =
	//	vtkSmartPointer<vtkSphereSource>::New();
	//sphereSource2->SetCenter(icp->GetSourceCenter());
	//sphereSource2->SetRadius(LENGTH * 0.1);


	//vtkSmartPointer<vtkPolyDataMapper> sphereMapper2 =
	//	vtkSmartPointer<vtkPolyDataMapper>::New();
	//sphereMapper2->SetInputConnection(sphereSource2->GetOutputPort());

	//vtkSmartPointer<vtkActor> sphereActor2 =
	//	vtkSmartPointer<vtkActor>::New();
	//sphereActor2->SetMapper(sphereMapper2);
	//sphereActor2->GetProperty()->SetColor(0, 1, 0);


	//vtkSmartPointer<vtkLineSource> lineSource1 =
	//	vtkSmartPointer<vtkLineSource>::New();
	//lineSource1->SetPoint1(icp->GetTargetCenter());
	//lineSource1->SetPoint2(
	//	icp->GetTargetCenter()[0] + LENGTH * icp->GetTargetDirection0()[0], 
	//	icp->GetTargetCenter()[1] + LENGTH * icp->GetTargetDirection0()[1], 
	//	icp->GetTargetCenter()[2] + LENGTH * icp->GetTargetDirection0()[2]);
	//lineSource1->Update();

	//vtkSmartPointer<vtkPolyDataMapper> lineMapper1 =
	//	vtkSmartPointer<vtkPolyDataMapper>::New();
	//lineMapper1->SetInputConnection(lineSource1->GetOutputPort());
	//
	//vtkSmartPointer<vtkActor> lineActor1 =
	//	vtkSmartPointer<vtkActor>::New();
	//lineActor1->SetMapper(lineMapper1);


	//	vtkSmartPointer<vtkLineSource> lineSource2 =
	//	vtkSmartPointer<vtkLineSource>::New();
	//lineSource2->SetPoint1(icp->GetSourceCenter());
	//lineSource2->SetPoint2(
	//	icp->GetSourceCenter()[0] + LENGTH * icp->GetSourceDirection0()[0], 
	//	icp->GetSourceCenter()[1] + LENGTH * icp->GetSourceDirection0()[1], 
	//	icp->GetSourceCenter()[2] + LENGTH * icp->GetSourceDirection0()[2]);
	//lineSource2->Update();

	//vtkSmartPointer<vtkPolyDataMapper> lineMapper2 =
	//	vtkSmartPointer<vtkPolyDataMapper>::New();
	//lineMapper2->SetInputConnection(lineSource2->GetOutputPort());
	//
	//vtkSmartPointer<vtkActor> lineActor2 =
	//	vtkSmartPointer<vtkActor>::New();
	//lineActor2->SetMapper(lineMapper2);
	//lineActor2->SetUserTransform(icp);

	//vtkSmartPointer<vtkRenderer> renderer =
	//	vtkSmartPointer<vtkRenderer>::New();
	//renderer->AddActor(actor1);
	//renderer->AddActor(actor2);
	//renderer->AddActor(actor3);
	//renderer->AddActor(lineActor1);
	//renderer->AddActor(lineActor2);
	//renderer->AddActor(sphereActor1);
	//renderer->AddActor(sphereActor2);


	//vtkSmartPointer<vtkRenderWindow> renderwindow =
	//	vtkSmartPointer<vtkRenderWindow>::New();
	//renderwindow->AddRenderer(renderer);

	//vtkSmartPointer<vtkRenderWindowInteractor> interactor =
	//	vtkSmartPointer<vtkRenderWindowInteractor>::New();
	//interactor->SetRenderWindow(renderwindow);
	//interactor->Start();


}