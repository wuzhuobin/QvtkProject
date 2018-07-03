// me
#include "QvtkInteractorStyleWindowLevel.h"
#include "QvtkScene.h"
#include "QvtkImage.h"
#include "QvtkImageSlice.h"
#include "QvtkProp.h"
#include "QvtkOrthogonalViewer.h"
#include "ui_QvtkInteractorStyleWindowLevel.h"

// vtk
#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include <vtkAbstractPicker.h>

// qt 
#include <QDebug>

namespace Q {
	namespace vtk {

vtkStandardNewMacro(InteractorStyleWindowLevel);
Image* InteractorStyleWindowLevel::pokedImage = nullptr;
void InteractorStyleWindowLevel::setCustomEnable(bool flag)
{
	UniqueUiInteractorObserver::setCustomEnable(flag);
	InteractorStyleWindowLevel::pokedImage = nullptr;

}

double InteractorStyleWindowLevel::GetWindow() 
{
	//SCBImage* image = this->FindPokedImage();
	if (!InteractorStyleWindowLevel::pokedImage) {
		qWarning()<< "Cannot poke an image";
		return 0.0;
	}
	return InteractorStyleWindowLevel::pokedImage->getWindow();
}

double InteractorStyleWindowLevel::GetLevel()
{
	if (!InteractorStyleWindowLevel::pokedImage) {
		qWarning()<< "Cannot poke an image";
		return 0.0;
	}
	return InteractorStyleWindowLevel::pokedImage->getLevel();
}

double InteractorStyleWindowLevel::GetResetWindow()
{
	if (!InteractorStyleWindowLevel::pokedImage) {
		qWarning()<< "Cannot poke an image";
		return 0.0;
	}
	const double* range = InteractorStyleWindowLevel::pokedImage->getImageData()->GetScalarRange();
	return (range[1] - range[0]) ;
}

double InteractorStyleWindowLevel::GetResetLevel()
{
	if (!InteractorStyleWindowLevel::pokedImage) {
		qWarning()<< "Cannot poke an image";
		return 0.0;
	}
	const double* range = InteractorStyleWindowLevel::pokedImage->getImageData()->GetScalarRange();
	return (range[1] + range[0]) / 2;
}

//#include "SCBOrthogonalImageViewer.h"
void InteractorStyleWindowLevel::SynchronalRender()
{
	synchronalCall<InteractorStyleWindowLevel>([](InteractorStyleWindowLevel* style) {
		style->GetInteractor()->Render();
	});
}

void InteractorStyleWindowLevel::SetWindow(double window)
{
	if (!InteractorStyleWindowLevel::pokedImage) {
		qWarning()<< "Cannot poke an image";
		return;
	}
	
	if (window == GetWindow()) {
		return;
	}

	//if (qRound(window) > this->ui->spinBoxWindow->maximum()) {
	//	this->ui->spinBoxWindow->setMaximum(qRound(window));
	//	this->ui->horizontalSliderWindow->setMaximum(qRound(window));
	//}
	//if (qRound(window) < this->ui->spinBoxWindow->minimum()) {
	//	this->ui->spinBoxWindow->setMinimum(qRound(window));
	//	this->ui->horizontalSliderWindow->setMinimum(qRound(window));
	//}

	this->ui->spinBoxWindow->setValue(qRound(window));
	InteractorStyleWindowLevel::pokedImage->setWindow(window);
	SynchronalRender();
	//Render();
}

void InteractorStyleWindowLevel::SetLevel(double level)
{
	if (!InteractorStyleWindowLevel::pokedImage) {
		qWarning()<< "Cannot poke an image";
		return;
	}

	if (level == GetLevel()) {
		return;
	}

	//if (qRound(level) > this->ui->spinBoxLevel->maximum()) {
	//	this->ui->spinBoxLevel->setMaximum(qRound(level));
	//	this->ui->horizontalSliderLevel->setMaximum(qRound(level));
	//}
	//if (qRound(level) < this->ui->spinBoxLevel->minimum()) {
	//	this->ui->spinBoxLevel->setMinimum(qRound(level));
	//	this->ui->horizontalSliderLevel->setMinimum(qRound(level));
	//}

	this->ui->spinBoxLevel->setValue(qRound(level));
	InteractorStyleWindowLevel::pokedImage->setLevel(level);
	SynchronalRender();
	//Render();
}

InteractorStyleWindowLevel::InteractorStyleWindowLevel()
{
	InteractorStyleWindowLevel::pokedImage = nullptr;
}

InteractorStyleWindowLevel::~InteractorStyleWindowLevel()
{
	InteractorStyleWindowLevel::pokedImage = nullptr;
}

void InteractorStyleWindowLevel::uniqueInstall()
{
	connect(this->ui->spinBoxWindow, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, static_cast<void(InteractorStyleWindowLevel::*)(int)>(&InteractorStyleWindowLevel::SetWindow));

	connect(this->ui->spinBoxLevel, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, static_cast<void(InteractorStyleWindowLevel::*)(int)>(&InteractorStyleWindowLevel::SetLevel));
}

int InteractorStyleWindowLevel::tryPick(double xyz[3]) 
{
	this->CurrentRenderer = this->GetDefaultRenderer();
	return this->GetInteractor()->GetPicker()->Pick(
		this->GetInteractor()->GetEventPosition()[0],
		this->GetInteractor()->GetEventPosition()[1],
		0,
		this->GetCurrentRenderer());
}

DataSet * InteractorStyleWindowLevel::findPokedDataSet()
{
	typedef QList<QMetaObject::Connection> ConnectionList;
	ConnectionList connections;
	QList<Q::vtk::Prop*> props = this->getViewer()->getProps();
	for (QList<Prop*>::const_iterator cit = props.cbegin();
		cit != props.cend(); ++cit) {
		ImageSlice* slice = qobject_cast<ImageSlice*>(*cit);
		if (slice) {
			// there is a connection for picked, when picked event, the picked Image pointer will be saved to 
			// pokedImage
			connections << connect(slice, &ImageSlice::picked,
				this, [this, slice]() {
				InteractorStyleOrthogonalViewer::pokedDataSet = slice->getRenderDataSet();
				InteractorStyleWindowLevel::pokedImage = qobject_cast<Image*>(slice->getRenderDataSet());
			});
		}
	}
	InteractorStyleWindowLevel::pokedImage = nullptr;
	InteractorStyleOrthogonalViewer::pokedDataSet = nullptr;
	this->tryPick();
	for (ConnectionList::const_iterator cit = connections.cbegin(); cit != connections.cend(); ++cit) {
		QObject::disconnect(*cit);
	}

	// if it cannot pick any Image, return the first Image in the current viewer.
	// if there is no image, the qobject_cast should return a nullptr.
	if (!InteractorStyleWindowLevel::pokedImage) {
		for (QList<Prop*>::const_iterator cit = props.cbegin(); cit != props.cend(); ++cit) {
			ImageSlice* slice = qobject_cast<ImageSlice*>(*cit);
			if (slice) {
				InteractorStyleOrthogonalViewer::pokedDataSet = slice->getRenderDataSet();
				InteractorStyleWindowLevel::pokedImage =
					qobject_cast<Image*>(slice->getRenderDataSet());
				break;
			}
		}
	}
	return InteractorStyleWindowLevel::pokedImage;

}

Image * InteractorStyleWindowLevel::FindPokedImage()
{

	//InteractorStyleWindowLevel::pokedImage = qobject_cast<Image*>(this->findPokedDataSet());

	//QList<Prop*> lists = this->getViewer()->getProps();
	//if (!InteractorStyleWindowLevel::pokedImage) {
	//	for (QList<Prop*>::const_iterator cit = lists.cbegin(); cit != lists.cend(); ++cit) {
	//		ImageSlice* slice = qobject_cast<ImageSlice*>(*cit);
	//		if (slice) {
	//			InteractorStyleWindowLevel::pokedImage =
	//				qobject_cast<Image*>(slice->getRenderDataSet());
	//		}
	//	}
	//}
	this->findPokedDataSet();
	return InteractorStyleWindowLevel::pokedImage;
}

void InteractorStyleWindowLevel::install()
{
	UNIQUE_UI_INSTALL(InteractorStyleWindowLevel);
}

void InteractorStyleWindowLevel::uninstall()
{
	UNIQUE_UI_UNINSTALL();
}

void InteractorStyleWindowLevel::OnLeftButtonDown()
{
	//this->CurrentRenderer = this->GetDefaultRenderer();
	if(this->FindPokedImage()){
		const double *scalarRange = InteractorStyleWindowLevel::pokedImage->getImageData()->GetScalarRange();
		this->getUi()->horizontalSliderLevel->setRange(qRound(scalarRange[0]), qRound(scalarRange[1]));
		this->getUi()->horizontalSliderWindow->setRange(qRound(scalarRange[0]), qRound(scalarRange[1]));
		this->getUi()->spinBoxLevel->setRange(qRound(scalarRange[0]), qRound(scalarRange[1]));
		this->getUi()->spinBoxWindow->setRange(qRound(scalarRange[0]), qRound(scalarRange[1]));
		vtkInteractorStyleWindowLevel::OnLeftButtonDown();
	}
	else
	{
		this->getUi()->horizontalSliderLevel->blockSignals(true);
		this->getUi()->horizontalSliderWindow->blockSignals(true);
		this->getUi()->spinBoxLevel->blockSignals(true);
		this->getUi()->spinBoxWindow->blockSignals(true);
		this->getUi()->horizontalSliderLevel->setRange(0, 0);
		this->getUi()->horizontalSliderWindow->setRange(0, 0);
		this->getUi()->spinBoxLevel->setRange(0, 0);
		this->getUi()->spinBoxWindow->setRange(0, 0);
		this->getUi()->horizontalSliderLevel->blockSignals(false);
		this->getUi()->horizontalSliderWindow->blockSignals(false);
		this->getUi()->spinBoxLevel->blockSignals(false);
		this->getUi()->spinBoxWindow->blockSignals(false);
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}
	// Lowering update rate. 
	FunctionSet2 f = [](InteractorObserver *ob) {
		static_cast<InteractorStyleWindowLevel*>(ob)->StartInteraction();
	};
	synchronalCall(this, f);
}

void InteractorStyleWindowLevel::OnLeftButtonUp()
{
	vtkInteractorStyleWindowLevel::OnLeftButtonUp();
	// Restoring update rate.
		FunctionSet2 f = [](InteractorObserver *ob) {
			InteractorStyleWindowLevel *style = static_cast<InteractorStyleWindowLevel*>(ob);
			style->EndInteraction();
			style->GetInteractor()->Render();
	};
	synchronalCall(this, f);
}

void InteractorStyleWindowLevel::SetWindow(int window)
{
	SetWindow(static_cast<double>(window));
}

void InteractorStyleWindowLevel::SetLevel(int level)
{
	SetLevel(static_cast<double>(level));
}
	}
}
