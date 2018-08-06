// me
#include "QvtkPolyDataSourceNormal.h"
#include "QvtkPolyDataSource.h"
#include "QvtkOrthogonalViewer.h"
#include "QvtkPlanarViewer.h"
#include "vtkBrokenLineSource.h"
// vtk
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCallbackCommand.h>
// qt
#include <QDebug>
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
			//this->lineActor->SetUserMatrix(this->SourceWidgetPolyData->getUserMatrix());
			this->lineActor->SetUserMatrix(this->Prop3D->GetUserMatrix());
			this->lineActor->SetScale(10.0);
			PlanarViewer *planarViewer = qobject_cast<PlanarViewer*>(this->getViewer());
			if (planarViewer) {
				planarViewer->GetAnnotationRenderer()->AddActor(this->lineActor);
				this->lineActor->GetUserMatrix()->AddObserver(vtkCommand::ModifiedEvent, this->matrixCallback);
				connect(this->getViewer(), &OrthogonalViewer::cursorPositionChanged,
					this, &PolyDataSourceNormal::cursorChange);
			}
			else {
				this->getViewer()->getRenderers()[0]->AddActor(this->lineActor);
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
			disconnect(this->getViewer(), &OrthogonalViewer::cursorPositionChanged,
				this, &PolyDataSourceNormal::cursorChange);
			this->lineActor->GetUserMatrix()->RemoveObserver(this->matrixCallback);
		}
		else {
			this->getViewer()->getRenderers()[0]->AddActor(this->lineActor);
		}
		this->lineActor->SetUserMatrix(nullptr);
	}
	this->lineActor->SetVisibility(flag);
}

void Q::vtk::PolyDataSourceNormal::cursorChange(double x, double y, double z)
{
	PolyDataSourceNormal::matrixModified(this->lineActor->GetUserMatrix(), vtkCommand::ModifiedEvent, this, nullptr);
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
	this->matrixCallback = vtkCallbackCommand::New();
	this->matrixCallback->SetClientData(this);
}

Q::vtk::PolyDataSourceNormal::~PolyDataSourceNormal()
{
	this->lineActor->Delete();
	this->lineMapper->Delete();
	this->brokenLine->Delete();
	this->matrixCallback->Delete();
}

void Q::vtk::PolyDataSourceNormal::matrixModified(vtkObject * vtkNotUsed(caller), unsigned long vtkNotUsed(eid), void * clientdata, void * vtkNotUsed(calldata))
{
	PolyDataSourceNormal *self = static_cast<PolyDataSourceNormal*>(clientdata);
	vtkMatrix4x4 *matrix = self->lineActor->GetUserMatrix();
	int orientation = self->getViewer()->getOrientation();
	const double *pos = self->getViewer()->getCursorPosition();
	double bounds[6];
	self->GetInput()->GetBounds(bounds);
	double _bounds1[4]{ bounds[0], bounds[2], bounds[4], 1 };
	double _bounds2[4]{ bounds[1], bounds[3], bounds[5], 1 };
	double real_bounds1[4];
	matrix->MultiplyPoint(_bounds1, real_bounds1);
	double real_bounds2[4];
	matrix->MultiplyPoint(_bounds2, real_bounds2);

	// when this poly data source is cutted by the viewer, showing its normal. 
	// when the cutting plane is in the middle of its plane. 
	bool show = (real_bounds1[orientation] < real_bounds2[orientation]) ?
		(pos[orientation] <= real_bounds2[orientation] && pos[orientation] >= real_bounds1[orientation]) :
		(pos[orientation] <= real_bounds1[orientation] && pos[orientation] >= real_bounds2[orientation]);
	self->lineActor->SetVisibility(show);
}

