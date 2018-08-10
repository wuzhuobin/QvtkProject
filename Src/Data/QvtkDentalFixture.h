#ifndef __QVTK_DENTAL_FIXTURE_H__
#define __QVTK_DENTAL_FIXTURE_H__
#pragma once
#include "QvtkDentalImplant.h"
namespace Q {
	namespace vtk {
		class DATA_EXPORT DentalFixture : public DentalImplant
		{
			Q_OBJECT;
			Q_VTK_DATA_H(
				DentalFixture,
				Q_VTK_KEY(DentalFixtureBrand)
				Q_VTK_KEY(DentalFixtureModel)
				Q_VTK_KEY(DentalFixtureShape)
				Q_VTK_KEY(DentalFixtureLength)
				Q_VTK_KEY(DentalFixtureRadius)
			);
		public:

			DentalFixture();
			virtual ~DentalFixture() override;
			virtual void printSelf() const override { DentalImplant::printSelf(); }

			virtual bool readData(QString rootDirectory = QString()) override;

			virtual QString getDentalFixtureBrand() const;
			virtual QString getDentalFixtureModel() const;
			virtual QString getDentalFixtureShape() const;
			virtual double getDentalFixtureLength() const;
			virtual double getDentalFixtureRadius() const;

			public slots:
			virtual void setDentalFixtureBrand(QString brand);
			virtual void setDentalFixtureModel(QString model);
			virtual void setDentalFixtureShape(QString shape);
			virtual void setDentalFixtureLength(double length);
			virtual void setDentalFixtureRadius(double radius);

		signals:
			void dentalFixtureBrandChanged(QString brand);
			void dentalFixtureModelChanged(QString model);
			void dentalFixtureShapeChanged(QString shape);
			void dentalFixtureLengthChanged(double length);
			void dentalFixtureRadiusChanged(double radius);

		protected:
			virtual Data* newInstance() const override { return new DentalFixture; }
			static void setDentalFixtureBrand(Data* data, QStandardItem* item);
			static void setDentalFixtureModel(Data* data, QStandardItem* item);
			static void setDentalFixtureShape(Data* data, QStandardItem* item);
			static void setDentalFixtureLength(Data* data, QStandardItem* item);
			static void setDentalFixtureRadius(Data* data, QStandardItem* item);

		private:

			QStandardItem *m_dentalFixtureBrand;
			QStandardItem *m_dentalFixtureModel;
			QStandardItem *m_dentalFixtureShape;
			QStandardItem *m_dentalFixtureLength;
			QStandardItem *m_dentalFixtureRadius;

		};
	}
}
#endif // !__QVTK_DENTAL_FIXTURE_H__
