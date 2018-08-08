#ifndef __QVTK_WIDGET_ORTHOGONAL_VIEWER_H__
#define __QVTK_WIDGET_ORTHOGONAL_VIEWER_H__
// me
#include "widget_export.h"
namespace Q {
	namespace vtk {
class OrthogonalViewer;
class WIDGET_EXPORT WidgetOrthogonalViewer 
{
public:
	virtual void setViewer(OrthogonalViewer* viewer) { this->m_viewer = viewer; }
	virtual OrthogonalViewer* getViewer() const { return this->m_viewer; }
protected:
	OrthogonalViewer* m_viewer;
};
	}
}
#endif // !__QVTK_WIDGET_ORTHOGONAL_VIEWER_H__
