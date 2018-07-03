//// me
//#include "QvtkBiopsyData.h"
//
//// vtk
//#include <vtkFieldData.h>
//#include <vtkDoubleArray.h>
//#include <vtkSmartPointer.h>
//
//// qt
//#include <QDomElement>
//namespace Q {
//	namespace vtk {
//
//		Q_VTK_DATA_CPP(BiopsyData);
//		BiopsyData::BiopsyData()
//		{
//			this->m_radius = 0.5;
//		}
//
//		BiopsyData::~BiopsyData()
//		{
//		}
//
//		void BiopsyData::printSelf() const
//		{
//			PolyData::printSelf();
//		}
//
//		void BiopsyData::readXML(const QDomElement & xml, QString directoryPath)
//		{
//			PolyData::readXML(xml, directoryPath);
//			this->setRadius(xml.attribute(K.Radius).toDouble());
//		}
//
//		void BiopsyData::writeXML(QDomElement & xml, QString directoryPath) const
//		{
//			PolyData::writeXML(xml, directoryPath);
//			xml.setAttribute(K.Radius, this->getRadius());
//		}
//
//		bool BiopsyData::writeData(QString rootDirectory) const
//		{
//			vtkSmartPointer<vtkDoubleArray> radiusArray =
//				vtkSmartPointer<vtkDoubleArray>::New();
//			radiusArray->SetName(K.Radius.toStdString().c_str());
//			radiusArray->InsertValue(0, this->m_radius);
//			this->getPolyData()->GetFieldData()->AddArray(radiusArray);
//			return PolyData::writeData(rootDirectory);
//		}
//	}
//}
