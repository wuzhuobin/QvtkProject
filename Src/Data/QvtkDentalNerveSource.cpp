#include "QvtkDentalNerveSource.h"

#include "vtkPoints.h"
#include "vtkParametricSpline.h"
#include "vtkParametricFunctionSource.h"
#include "vtkTubeFilter.h"
#include "vtkPolyData.h"

#include <QDomElement>

namespace Q {
	namespace vtk {


Q_VTK_DATA_CPP(DentalNerveSource);
DentalNerveSource::DentalNerveSource()
{
	m_splinePoints = vtkPoints::New();
	m_nervePolyData = vtkPolyData::New();
	m_radius = 1;
	m_resolution = 50;
}

void DentalNerveSource::printSelf() const
{
	AnnotationPolyData::printSelf();
}

bool DentalNerveSource::readData(QString rootDirectory)
{
	return 0;
}

bool DentalNerveSource::writeData(QString rootDirectory) const
{
	return 0;
}

void DentalNerveSource::setNerveId(unsigned int nerveId)
{
	m_nerveId = nerveId;
}

unsigned int DentalNerveSource::getNerveId()
{
	return m_nerveId;
}

void DentalNerveSource::addPoint(double point[3])
{
	m_splinePoints->InsertNextPoint(point[0], point[1], point[2]);
	this->compute();
}

void DentalNerveSource::removePoint(unsigned int id)
{
	vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();

	for (int i = 0; i < m_splinePoints->GetNumberOfPoints(); i++)
	{
		if (i != id)
		{
			double point[3];
			m_splinePoints->GetPoint(i, point);
			newPoints->InsertNextPoint(point);
		}
	}

	m_splinePoints->ShallowCopy(newPoints);
	this->compute();
}

void DentalNerveSource::setPoint(unsigned int id, double point[3])
{
	m_splinePoints->SetPoint(id, point[0], point[1], point[2]);
	this->compute();
}

void DentalNerveSource::clear()
{
	m_splinePoints->Reset();
	this->compute();
}

void DentalNerveSource::getPoints(vtkPoints * points)
{
	points->ShallowCopy(m_splinePoints);
}

vtkPoints* DentalNerveSource::getPoints()
{
	return m_splinePoints;
}

void DentalNerveSource::getPolyData(vtkPolyData * nerve)
{
	nerve->DeepCopy(m_nervePolyData);
}

vtkPolyData * DentalNerveSource::getPolyData()
{
	return m_nervePolyData;
}

void DentalNerveSource::setRadius(double radius)
{
	m_radius = radius;
	emit radiusChanged(radius);
}

void DentalNerveSource::setResolution(double resolution)
{
	m_resolution = resolution;
	emit resolutionChanged(resolution);
}

double DentalNerveSource::getRadius()
{
	return m_radius;
}

double DentalNerveSource::getResolution()
{
	return m_resolution;
}

void DentalNerveSource::compute()
{
	vtkSmartPointer<vtkParametricSpline> spline = vtkSmartPointer<vtkParametricSpline>::New();
	spline->SetPoints(m_splinePoints);

	vtkSmartPointer<vtkParametricFunctionSource> splineFunctionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
	splineFunctionSource->SetParametricFunction(spline);
	splineFunctionSource->Update();

	vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
	tubeFilter->SetInputConnection(splineFunctionSource->GetOutputPort());
	tubeFilter->SetRadius(m_radius);
	tubeFilter->SetNumberOfSides(m_resolution);
	tubeFilter->CappingOn();
	tubeFilter->Update();

	m_nervePolyData->DeepCopy(tubeFilter->GetOutput());

	emit updateComplete();
}

DentalNerveSource::~DentalNerveSource()
{
	m_splinePoints->Delete();
	m_nervePolyData->Delete();
}

void DentalNerveSource::update()
{
	this->compute();
}
	}
}