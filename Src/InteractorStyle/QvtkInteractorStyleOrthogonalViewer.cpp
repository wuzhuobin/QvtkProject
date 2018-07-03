// me 
#include "QvtkInteractorStyleOrthogonalViewer.h"
#include "QvtkDataSet.h"
#include "QvtkProp.h"
#include "QvtkOrthogonalViewer.h"
// vtk 
#include <vtkInteractorStyle.h>
// qt 
#include <QDebug>
namespace Q {
	namespace vtk {
		DataSet *InteractorStyleOrthogonalViewer::pokedDataSet = nullptr;
		DataSet * InteractorStyleOrthogonalViewer::findPokedDataSet()
		{
			typedef QList<QMetaObject::Connection> ConnectionList;
			ConnectionList connections;
			QList<Q::vtk::Prop*> props = this->getViewer()->getProps();
			for (QList<Prop*>::const_iterator cit = props.cbegin();
				cit != props.cend(); ++cit) {
					// there is a connection for picked, when picked event, the picked Image pointer will be saved to 
					// pokedDataSet
					connections << QObject::connect(*cit, &Prop::picked,
						[this, cit]() {
						InteractorStyleOrthogonalViewer::pokedDataSet = (*cit)->getRenderDataSet();});
			}
			InteractorStyleOrthogonalViewer::pokedDataSet = nullptr;
			this->tryPick();
			for (ConnectionList::const_iterator cit = connections.cbegin(); cit != connections.cend(); ++cit) {
				QObject::disconnect(*cit);
			}
			return InteractorStyleOrthogonalViewer::pokedDataSet;
		}
		int InteractorStyleOrthogonalViewer::tryPick(double xyz[3])
		{
			qWarning() << "Not implemented.";
			return 0;
		}
	}
}
