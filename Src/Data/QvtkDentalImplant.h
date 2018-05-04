#ifndef __QVTK_DENTAL_IMPLANT_H__
#define __QVTK_DENTAL_IMPLANT_H__

// me
#include "QvtkAnnotationPolyData.h"

namespace Q {
	namespace vtk {
		class DentalImplant : public AnnotationPolyData
		{
			Q_OBJECT;
			Q_VTK_DATA_H(
				DentalImplant,
				Q_VTK_KEY(ToothPositionOrientation)
				Q_VTK_KEY(ToothPositionSide)
				Q_VTK_KEY(ToothPositionIndex)
			);
		public:
			DentalImplant();
			virtual ~DentalImplant() override;
			virtual void printSelf() const override;
			struct ToothPosition
			{
				enum ORIENTATION
				{
					UPPER = 0,
					LOWER
				};

				enum SIDE
				{
					LEFT = 0,
					RIGHT
				};

				enum INDEX
				{
					FIRST = 0,
					SECOND,
					THIRD,
					FORTH,
					FIFTH,
					SIXTH,
					SEVENTH,
					EIGTHTH
				};

				unsigned short index = FIRST;
				bool orientation = UPPER;
				bool side = LEFT;
				unsigned int toFDI() const;
				void fromFDI(unsigned int i);
			};

			virtual void readXML(const QDomElement& xml, QString directoryPath = QString()) override;
			virtual void writeXML(QDomElement& xml, QString directoryPath = QString()) const override;

			virtual ToothPosition getToothPosition() { return this->m_toothPosition; }
			virtual const ToothPosition &getToothPosition() const { return this->m_toothPosition; }

			public slots:
			virtual void setToothPosition(ToothPosition toothPosition);

		signals:
			void toothPositionChanged(const ToothPosition& toothPosition);

		protected:
			virtual Data* newInstance() const override = 0;

		private:
			ToothPosition m_toothPosition;

		};
	}
}
#endif // !__QVTK_DENTAL_IMPLANT_H__