#ifndef __QVTK_BIOPSY_DATA_H__
#define __QVTK_BIOPSY_DATA_H__
#pragma once
#include "QvtkPolyData.h"
namespace Q {
	namespace vtk {

//		class BiopsyData :
//			public PolyData
//		{
//			Q_OBJECT;
//			Q_VTK_DATA_H(
//				BiopsyData,
//				Q_VTK_KEY(Radius)
//			)
//		public:
//			BiopsyData();
//			virtual ~BiopsyData() override;
//			virtual void printSelf() const override;
//
//			virtual void readXML(const QDomElement& xml, QString directoryPath = QString()) override;
//			virtual void writeXML(QDomElement& xml, QString directoryPath = QString()) const override;
//
//			virtual bool writeData(QString rootDirectory = QString()) const override;
//
//			virtual double getRadius() const { return this->m_radius; }
//			public slots:
//			virtual void setRadius(double radius) { this->m_radius = radius; }
//
//		protected:
//			virtual BiopsyData* newInstance() const override { return new BiopsyData; }
//
//		private:
//			double m_radius;
//
//		};
class BiopsyData : public PolyData
{
	Q_OBJECT;
	Q_VTK_DATA_H(BiopsyData);
protected:
	virtual BiopsyData *newInstance() const override { return new BiopsyData; }
};
	}
}
////class BiopsyData: 
//
#endif // !__QVTK_BIOPSY_DATA_H__
