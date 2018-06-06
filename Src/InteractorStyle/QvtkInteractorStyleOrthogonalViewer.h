#ifndef __QVTK_INTERACTOR_STYLE_ORTHOGONAL_VIEWER_H__
#define __QVTK_INTERACTOR_STYLE_ORTHOGONAL_VIEWER_H__
#pragma once
// me
namespace Q {
	namespace vtk {
		class OrthogonalViewer;
		class DataSet;
	}
}
namespace Q {
	namespace vtk {
		class InteractorStyleOrthogonalViewer
		{
		public:
			virtual void setViewer(OrthogonalViewer* viewer) { this->m_viewer = viewer; }
			virtual OrthogonalViewer* getViewer() const { return this->m_viewer; }
		protected:
			OrthogonalViewer* m_viewer;
			virtual DataSet *findPokedDataSet();
			virtual int tryPick(double xyz[3] = nullptr);
			static DataSet *pokedDataSet;
		};
	}
}

#endif // !__QVTK_INTERACTOR_STYLE_ORTHOGONAL_VIEWER_H__
