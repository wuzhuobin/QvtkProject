#ifndef __QVTK_INTERACTOR_STYLE_ORTHOGONAL_VIEWER_H__
#define __QVTK_INTERACTOR_STYLE_ORTHOGONAL_VIEWER_H__
#pragma once
// me
#include "interactorstyle_export.h"
namespace Q {
	namespace vtk {
		class OrthogonalViewer;
		class DataSet;
	}
}
namespace Q {
	namespace vtk {
		class INTERACTORSTYLE_EXPORT InteractorStyleOrthogonalViewer
		{
		public:
			virtual void setViewer(OrthogonalViewer* viewer) { this->m_viewer = viewer; }
			virtual OrthogonalViewer* getViewer() const { return this->m_viewer; }
		protected:
			OrthogonalViewer* m_viewer;
			virtual DataSet *findPokedDataSet(const int XY[2] = nullptr);
			virtual int tryPick(const int XY[2] = nullptr);
			static DataSet *pokedDataSet;
		};
	}
}

#endif // !__QVTK_INTERACTOR_STYLE_ORTHOGONAL_VIEWER_H__
