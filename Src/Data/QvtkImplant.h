#ifndef __QVTK_IMPLANT_H__
#define __QVTK_IMPLANT_H__
#pragma once;
// me
#include "data_export.h"
#include "QvtkAnnotationPolyData.h"
namespace Q {
	namespace vtk {
		class DATA_EXPORT Implant : public AnnotationPolyData
		{
			Q_OBJECT;
			Q_VTK_DATA_H(
				Implant,
				Q_VTK_KEY(ImplantSource)
			)
		public:
			Implant();
			virtual ~Implant() override;
			virtual void printSelf() const override;

			virtual bool readData(QString rootDirectory = QString()) override;
			//virtual bool writeData(QString rootDirectory = QString()) override;

			virtual unsigned int getImplantSource() const;

			public slots:

			virtual void setImplantSource(unsigned int i);

		protected:

			virtual Data* newInstance() const override { return new Implant; }

			static void setImplantSource(Data* self, QStandardItem* item);

			QStandardItem* m_implantSource;

		};

	}
}

#endif // !__QVTK_IMPLANT_H__
