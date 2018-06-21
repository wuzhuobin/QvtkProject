// me
#include "QvtkPolyDataSourceNormal.h"
#include "QvtkPolyDataSource.h"
#include "QvtkOrthogonalViewer.h"
#include "QvtkPlanarViewer.h"
#include "vtkBrokenLineFilter.h"
// vtk
//#include <vtkLineSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
vtkStandardNewMacro(Q::vtk::PolyDataSourceNormal)
void Q::vtk::PolyDataSourceNormal::PrintSelf(ostream & os, vtkIndent indent)
{
}

void Q::vtk::PolyDataSourceNormal::setCustomEnable(bool flag)
{
	Superclass::setCustomEnable(flag);
	if (flag) {
		if (this->SourceWidgetPolyData) {
			double bounds[6];
			//this->SourceWidgetPolyData->getWorldBounds(bounds);
			this->GetInput()->GetBounds(bounds);
			this->brokenLine->SetPoint1(0, 0, bounds[4]);
			this->brokenLine->SetPoint2(0, 0, bounds[5]);
			this->brokenLine->Update();
			this->lineActor->SetUserMatrix(this->SourceWidgetPolyData->getUserMatrix());
			this->lineActor->SetScale(10.0);
			PlanarViewer *planarViewer = qobject_cast<PlanarViewer*>(this->getViewer());
			if (planarViewer) {
				planarViewer->GetAnnotationRenderer()->AddActor(this->lineActor);
			}
			else {
				this->getViewer()->GetRenderers()[0]->AddActor(this->lineActor);
			}
		}
		else
		{
			vtkErrorMacro(<< "this->SourceWidgetPolyData is a nullptr. " << "Please SetPolyData(). ");
		}
	}
	else {
		PlanarViewer *planarViewer = qobject_cast<PlanarViewer*>(this->getViewer());
		if (planarViewer) {
			planarViewer->GetAnnotationRenderer()->AddActor(this->lineActor);
		}
		else {
			this->getViewer()->GetRenderers()[0]->AddActor(this->lineActor);
		}
		this->lineActor->SetUserMatrix(nullptr);
	}
	this->lineActor->SetVisibility(flag);
}

Q::vtk::PolyDataSourceNormal::PolyDataSourceNormal()
{
	this->brokenLine = vtkBrokenLineSource::New();
	this->brokenLine->SetResolution(99);
	this->brokenLine->SetSolid(5);
	this->brokenLine->SetEmpty(5);
	this->lineMapper = vtkPolyDataMapper::New();
	this->lineMapper->SetInputConnection(this->brokenLine->GetOutputPort());
	this->lineActor = vtkActor::New();
	this->lineActor->SetMapper(this->lineMapper);
	this->lineActor->GetProperty()->SetColor(0, 1, 0);
}

Q::vtk::PolyDataSourceNormal::~PolyDataSourceNormal()
{
	this->lineActor->Delete();
	this->lineMapper->Delete();
	this->brokenLine->Delete();
}
