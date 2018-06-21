#ifndef __QVTK_POLY_DATA_SOURCE_NORMAL_H__
#define __QVTK_POLY_DATA_SOURCE_NORMAL_H__
#pragma once
// me
#include "QvtkPolyDataSourceWidget.h"
// vtk
class vtkPolyDataMapper;
class vtkActor;
class vtkBrokenLineSource;
namespace Q {
	namespace vtk {
		class PolyDataSourceNormal : public PolyDataSourceWidget {
			Q_OBJECT;
		public:
			static PolyDataSourceNormal *New();
			vtkTypeMacro(PolyDataSourceNormal, PolyDataSourceWidget);
			virtual void PrintSelf(ostream &os, vtkIndent indent) override;
			virtual void setCustomEnable(bool flag) override;
		protected:
			PolyDataSourceNormal();
			virtual ~PolyDataSourceNormal() override;
			vtkBrokenLineSource *brokenLine;
			vtkPolyDataMapper *lineMapper;
			vtkActor *lineActor;
		private:
			Q_DISABLE_COPY(PolyDataSourceNormal);
		};
	}
}
#endif // !__QVTK_POLY_DATA_SOURCE_NORMAL_H__
