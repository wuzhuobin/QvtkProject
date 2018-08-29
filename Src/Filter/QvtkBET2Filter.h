#ifndef __QVTK_BET2_FILTER_H__
#define __QVTK_BET2_FILTER_H__
#pragma once
// me
#include "filter_export.h"
#include "QvtkFilter2.h"
#include "ui_QvtkBET2Filter.h"
#include "vtkBet2Filter.h"
namespace Q {
	namespace vtk {
		class FILTER_EXPORT BET2Filter :
			public Filter2,  
			public Ui::BET2Filter,
			public vtkBet2Filter 
		{
			Q_OBJECT;
		public:
			static BET2Filter *New();
			vtkTypeMacro(BET2Filter, vtkBet2Filter);
			virtual void PrintSelf(ostream &os, vtkIndent indent) override;
			explicit BET2Filter(QWidget *parent = nullptr);
			virtual ~BET2Filter() override;
		public Q_SLOTS:
			virtual void Update() override;
		private:
			Q_DISABLE_COPY(BET2Filter);
		};
	}
}
#endif // !__QVTK_BET2_FILTER_H__
