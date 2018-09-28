// me 
#include "QvtkInteractorStyleOrthogonalViewer.h"
#include "QvtkDataSet.h"
#include "QvtkProp.h"
#include "QvtkOrthogonalViewer.h"
// vtk 
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyle.h>
#include <vtkAbstractPicker.h>
// qt 
#include <QDebug>
namespace Q {
	namespace vtk {
		DataSet *InteractorStyleOrthogonalViewer::pokedDataSet = nullptr;
		DataSet * InteractorStyleOrthogonalViewer::findPokedDataSet(const int XY[2]) 
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
			this->tryPick(XY);
			for (ConnectionList::const_iterator cit = connections.cbegin(); cit != connections.cend(); ++cit) {
				QObject::disconnect(*cit);
			}
			return InteractorStyleOrthogonalViewer::pokedDataSet;
		}
		int InteractorStyleOrthogonalViewer::tryPick(const int XY[2])
		{
			vtkInteractorStyle *style = dynamic_cast<vtkInteractorStyle*>(this);
			if (style) {
				style->SetCurrentRenderer(style->GetDefaultRenderer());
				if (XY == nullptr) {
					return style->GetInteractor()->GetPicker()->Pick(
						style->GetInteractor()->GetEventPosition()[0],
						style->GetInteractor()->GetEventPosition()[1],
						0,
						style->GetCurrentRenderer());
				}
				else {
					return style->GetInteractor()->GetPicker()->Pick(
						XY[0], 
						XY[1],
						0,
						style->GetCurrentRenderer());
				}
			}
			return 0;
		}
	}
}
