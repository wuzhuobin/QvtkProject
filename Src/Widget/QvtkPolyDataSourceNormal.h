#ifndef __QVTK_POLY_DATA_SOURCE_NORMAL_H__
#define __QVTK_POLY_DATA_SOURCE_NORMAL_H__
#pragma once
// me
#include "widget_export.h"
#include "QvtkPolyDataSourceWidget.h"
// vtk
class vtkPolyDataMapper;
class vtkActor;
class vtkBrokenLineSource;
class vtkCallbackCommand;
namespace Q {
	namespace vtk {
		class WIDGET_EXPORT PolyDataSourceNormal : public PolyDataSourceWidget {
			Q_OBJECT;
		public:
			static PolyDataSourceNormal *New();
			vtkTypeMacro(PolyDataSourceNormal, PolyDataSourceWidget);
			virtual void PrintSelf(ostream &os, vtkIndent indent) override;
			virtual void setCustomEnable(bool flag) override;
		protected slots:
			void cursorChange(double x, double y, double z);
		protected:
			PolyDataSourceNormal();
			virtual ~PolyDataSourceNormal() override;
			static void matrixModified(vtkObject* self, unsigned long eid, void* clientdata, void *calldata);
			vtkBrokenLineSource *brokenLine;
			vtkPolyDataMapper *lineMapper;
			vtkActor *lineActor;
			vtkCallbackCommand *matrixCallback;
		private:
			Q_DISABLE_COPY(PolyDataSourceNormal);
		};
	}
}
#endif // !__QVTK_POLY_DATA_SOURCE_NORMAL_H__
