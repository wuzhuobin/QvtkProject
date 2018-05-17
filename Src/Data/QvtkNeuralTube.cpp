// me
#include "QvtkNeuralTube.h"
//vtk
#include <vtkTubeFilter.h>
#include <vtkLineSource.h>
#include <vtkPolyData.h>
// qt
#include <QDomElement>
namespace Q {
	namespace vtk {

		Q_VTK_DATA_CPP(NeuralTube);
		NeuralTube::NeuralTube()
			:m_intermediatePoint{ 0,0,0 }
		{
			this->m_lineSource = vtkLineSource::New();
			this->m_lineSource->SetResolution(1);
			this->m_tubeFilter = vtkTubeFilter::New();
			this->m_tubeFilter->SetInputConnection(this->m_lineSource->GetOutputPort());
			this->m_tubeFilter->SetCapping(true);
			this->m_tubeFilter->SetRadius(0);
		}

		NeuralTube::~NeuralTube()
		{
			this->m_lineSource->Delete();
			this->m_tubeFilter->Delete();
		}

		void NeuralTube::readXML(const QDomElement & xml, QString directoryPath)
		{
			PolyData::readXML(xml, directoryPath);
			this->setTubeRadius(xml.attribute(K.TubeRadius).toDouble());
			this->setTubeLength(xml.attribute(K.TubeLength).toDouble());
			NeuralTube::readArray(K.IntermediatePoint, xml, this->m_intermediatePoint, 3);
			NeuralTube::readArray(K.EndPoint, xml, this->m_lineSource->GetPoint2(), 3);
		}

		void NeuralTube::writeXML(QDomElement & xml, QString directoryPath) const
		{
			PolyData::writeXML(xml, directoryPath);
			xml.setAttribute(K.TubeRadius, this->getTubeRadius());
			xml.setAttribute(K.TubeLength, this->getTubeLength());
			NeuralTube::writeArray(K.IntermediatePoint, xml, this->getIntermediatePoint(), 3);
			NeuralTube::writeArray(K.EndPoint, xml, this->getEndPoint(), 3);
		}

		bool NeuralTube::readData(QString rootDirectory)
		{
			if (rootDirectory.isEmpty()) {
				return PolyData::readData(rootDirectory);
			}
			else {
				double vector[3];
				vtkMath::Subtract(static_cast<const NeuralTube*>(this)->getIntermediatePoint(), static_cast<const NeuralTube*>(this)->getEndPoint(), vector);
				vtkMath::Normalize(vector);
				vtkMath::MultiplyScalar(vector, this->getTubeLength());
				vtkMath::Add(static_cast<const NeuralTube*>(this)->getEndPoint(), vector, vector);
				this->m_lineSource->SetPoint1(vector);
				this->m_lineSource->Update();
				int sides = this->getTubeRadius() / 1 + 10;
				this->m_tubeFilter->SetNumberOfSides(sides);
				this->m_tubeFilter->Update();
				this->getPolyData()->ShallowCopy(this->m_tubeFilter->GetOutput());
				return true;
			}
		}

		double NeuralTube::getTubeRadius() const
		{
			return this->m_tubeFilter->GetRadius();
		}

		const double * NeuralTube::getEndPoint() const
		{
			return this->m_lineSource->GetPoint2();
		}

		QList<double> NeuralTube::getEndPoint()
		{
			return QList<double>{this->getEndPoint()[0], this->getEndPoint()[1], this->getEndPoint()[2]};
		}

		void NeuralTube::setTubeLength(double length)
		{
			this->m_tubeLength = length;
			this->readData("...");
			emit this->tubeLengthChanged(length);
		}

		void NeuralTube::setTubeRadius(double radius)
		{
			this->m_tubeFilter->SetRadius(radius);
			this->readData("...");
			emit this->tubeRadiusChanged(radius);
		}

		void NeuralTube::setIntermediatePoint(double x, double y, double z)
		{
			this->m_intermediatePoint[0] = x;
			this->m_intermediatePoint[1] = y;
			this->m_intermediatePoint[2] = z;
			this->readData("...");
			emit this->intermediatePointChanged(this->getIntermediatePoint());
			emit this->intermediatePointChanged(this->m_intermediatePoint);
		}

		void NeuralTube::setEndPoint(double x, double y, double z)
		{
			this->m_lineSource->SetPoint2(x, y, z);
			this->readData("...");
			emit this->endPointChanged(this->getEndPoint());
			emit this->endPointChanged(static_cast<const NeuralTube*>(this)->getEndPoint());
		}
	}
}