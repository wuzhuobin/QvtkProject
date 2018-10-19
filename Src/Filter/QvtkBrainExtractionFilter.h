#ifndef __QVTK_BRAIN_EXTRACTION_FILTER_H__
#define __QVTK_BRAIN_EXTRACTION_FILTER_H__
// me
#include "filter_export.h"
#include "QvtkFilter2.h"
#include "ui_QvtkBrainExtractionFilter.h"
#include "vtkBrainExtractionFilter.h"
namespace Q {
	namespace vtk {
		class FILTER_EXPORT BrainExtractionFilter :
			public Filter2,  
			public Ui::BrainExtractionFilter,
			public vtkBrainExtractionFilter
		{
			Q_OBJECT;
		public:
			static BrainExtractionFilter *New();
			vtkTypeMacro(BrainExtractionFilter, vtkBrainExtractionFilter);
			virtual void PrintSelf(ostream &os, vtkIndent indent) override;
			explicit BrainExtractionFilter(QWidget *parent = nullptr);
			virtual ~BrainExtractionFilter() override;
		public Q_SLOTS:
			virtual void Update() override;
		private:
			Q_DISABLE_COPY(BrainExtractionFilter);
		};
	}
}
#endif // !__QVTK_BRAIN_EXTRACTION_FILTER_H__

