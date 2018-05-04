#ifndef __QVTK_INTERACTOR_STYLE_ORTHOGONAL_VIEWER_H__
#define __QVTK_INTERACTOR_STYLE_ORTHOGONAL_VIEWER_H__

// me
namespace Q {
	namespace vtk {
		class OrthogonalViewer;

	}
}

// vtk
#include <vtkInteractorStyle.h>
namespace Q {
	namespace vtk {

		class InteractorStyleOrthogonalViewer
		{
		public:
			virtual void setViewer(OrthogonalViewer* viewer) { this->m_viewer = viewer; }
			virtual OrthogonalViewer* getViewer() const { return this->m_viewer; }
		protected:
			OrthogonalViewer* m_viewer;
		};

	}
}

#endif // !__QVTK_INTERACTOR_STYLE_ORTHOGONAL_VIEWER_H__
