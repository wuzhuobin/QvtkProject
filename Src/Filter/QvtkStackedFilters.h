#ifndef __QVTK_STACKED_FILTERS_H__
#define __QVTK_STACKED_FILTERS_H__
#pragma once
// me 
#include "filter_export.h"
#include "QvtkBrainExtractionFilter.h"
// qt 
#include "QStackedWidget" 
namespace Q {
namespace vtk{
class FILTER_EXPORT StackedFilter : public QStackedWidget
{
	Q_OBJECT;
public:
	explicit StackedFilter(QWidget *parent = nullptr) {
		this->addWidget(&this->brainExtractionFilter);
	}
	BrainExtractionFilter *getBrainExtractionFilter() { return &this->brainExtractionFilter; }
protected:
	BrainExtractionFilter brainExtractionFilter;
};
}
}

#endif // !__QVTK_STACKED_FILTERS_H__
