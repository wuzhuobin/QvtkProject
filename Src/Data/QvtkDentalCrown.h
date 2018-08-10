#ifndef __QVTK_DENTAL_CROWN_H__
#define __QVTK_DENTAL_CROWN_H__ 
#pragma once
// me
#include "QvtkDentalImplant.h"
namespace Q {
	namespace vtk {
		class DATA_EXPORT DentalCrown : public DentalImplant
		{
			Q_OBJECT;
			Q_VTK_DATA_H(DentalCrown);

		public:
			DentalCrown();

			virtual void printSelf() const override { AnnotationPolyData::printSelf(); }

			virtual bool readData(QString rootDirectory = QString()) override;

			virtual void setToothPosition(ToothPosition toothPosition) override;

		protected:
			virtual Data* newInstance() const override { return new DentalCrown; }

		};
	}
}
#endif // __QVTK_DENTAL_CROWN_H__
