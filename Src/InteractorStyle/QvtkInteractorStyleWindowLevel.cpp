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
Image* InteractorStyleWindowLevel::pockedImage = nullptr;
void InteractorStyleWindowLevel::setCustomEnable(bool flag)
{
	UniqueUiInteractorObserver::setCustomEnable(flag);
	InteractorStyleWindowLevel::pockedImage = nullptr;

}

double InteractorStyleWindowLevel::GetWindow() 
{
	//SCBImage* image = this->FindPokedImage();
	if (!InteractorStyleWindowLevel::pockedImage) {
		qWarning()<< "Cannot poke an image";
		return 0.0;
	}
	return InteractorStyleWindowLevel::pockedImage->getWindow();
}

double InteractorStyleWindowLevel::GetLevel()
{
	if (!InteractorStyleWindowLevel::pockedImage) {
		qWarning()<< "Cannot poke an image";
		return 0.0;
	}
	return InteractorStyleWindowLevel::pockedImage->getLevel();
}

double InteractorStyleWindowLevel::GetResetWindow()
{
	if (!InteractorStyleWindowLevel::pockedImage) {
		qWarning()<< "Cannot poke an image";
		return 0.0;
	}
	const double* range = InteractorStyleWindowLevel::pockedImage->getImageData()->GetScalarRange();
	return (range[1] - range[0]) ;
}

double InteractorStyleWindowLevel::GetResetLevel()
{
	if (!InteractorStyleWindowLevel::pockedImage) {
		qWarning()<< "Cannot poke an image";
		return 0.0;
	}
	const double* range = InteractorStyleWindowLevel::pockedImage->getImageData()->GetScalarRange();
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
	if (!InteractorStyleWindowLevel::pockedImage) {
		qWarning()<< "Cannot poke an image";
		return;
	}
	
	if (window == GetWindow()) {
		return;
	}

	if (qRound(window) > this->ui->spinBoxWindow->maximum()) {
		this->ui->spinBoxWindow->setMaximum(qRound(window));
		this->ui->horizontalSliderWindow->setMaximum(qRound(window));
	}
	if (qRound(window) < this->ui->spinBoxWindow->minimum()) {
		this->ui->spinBoxWindow->setMinimum(qRound(window));
		this->ui->horizontalSliderWindow->setMinimum(qRound(window));
	}

	this->ui->spinBoxWindow->setValue(qRound(window));
	InteractorStyleWindowLevel::pockedImage->setWindow(window);
	SynchronalRender();
	//Render();
}

void InteractorStyleWindowLevel::SetLevel(double level)
{
	if (!InteractorStyleWindowLevel::pockedImage) {
		qWarning()<< "Cannot poke an image";
		return;
	}

	if (level == GetLevel()) {
		return;
	}

	if (qRound(level) > this->ui->spinBoxLevel->maximum()) {
		this->ui->spinBoxLevel->setMaximum(qRound(level));
		this->ui->horizontalSliderLevel->setMaximum(qRound(level));
	}
	if (qRound(level) < this->ui->spinBoxLevel->minimum()) {
		this->ui->spinBoxLevel->setMinimum(qRound(level));
		this->ui->horizontalSliderLevel->setMinimum(qRound(level));
	}

	this->ui->spinBoxLevel->setValue(qRound(level));
	InteractorStyleWindowLevel::pockedImage->setLevel(level);
	SynchronalRender();
	//Render();
}

InteractorStyleWindowLevel::InteractorStyleWindowLevel()
{
	InteractorStyleWindowLevel::pockedImage = nullptr;
}

InteractorStyleWindowLevel::~InteractorStyleWindowLevel()
{
	InteractorStyleWindowLevel::pockedImage = nullptr;
}

void InteractorStyleWindowLevel::uniqueInstall()
{
	connect(this->ui->spinBoxWindow, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, static_cast<void(InteractorStyleWindowLevel::*)(int)>(&InteractorStyleWindowLevel::SetWindow));

	connect(this->ui->spinBoxLevel, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, static_cast<void(InteractorStyleWindowLevel::*)(int)>(&InteractorStyleWindowLevel::SetLevel));
}

Image * InteractorStyleWindowLevel::FindPokedImage()
{
	typedef QList<QMetaObject::Connection> ConnectionList;
	ConnectionList connections;
	for (QList<Prop*>::const_iterator cit = this->getViewer()->GetProps().cbegin();
		cit != this->getViewer()->GetProps().cend(); ++cit) {
		ImageSlice* slice = qobject_cast<ImageSlice*>(*cit);
		if (slice) {
			//connections << connect(slice, &SCBImageSlice::picked,
			//	this, &InteractorStyleWindowLevel::ImagePicked);
			connections << connect(slice, &ImageSlice::picked,
				this, [this]() {
				ImageSlice* slice = qobject_cast<ImageSlice*>(this->sender());
				if (slice) {
					InteractorStyleWindowLevel::pockedImage = qobject_cast<Image*>(slice->getRenderDataSet());
					return;
				}
				InteractorStyleWindowLevel::pockedImage = nullptr;	});

		}
	}
	InteractorStyleWindowLevel::pockedImage = nullptr;
		//foreach(Prop* prop, this->getViewer()->GetProps()) {
		//	SCBImageSlice* slice = qobject_cast<SCBImageSlice*>(prop);
		//	if (slice) {
		//		disconnect(slice, &SCBImageSlice::picked,
		//			this, &InteractorStyleWindowLevel::ImagePicked);
		//	}
		//}
		//this->FindPokedRenderer(
		//this->GetInteractor()->GetEventPosition()[0],
		//this->GetInteractor()->GetEventPosition()[1]);
		this->CurrentRenderer = this->GetDefaultRenderer();
	// there is a connection for picked, when picked event, the picked Image pointer will be saved to 
	// this->Image
	int picked = this->GetInteractor()->GetPicker()->Pick(
		this->GetInteractor()->GetEventPosition()[0],
		this->GetInteractor()->GetEventPosition()[1],
		0,
		this->GetCurrentRenderer());
	// if it cannot pick any Image, return the first BImage in the current viewer.
	// if there is no image, the qobject_cast should return a nullptr.
	if (!InteractorStyleWindowLevel::pockedImage) {
		for(QList<Prop*>::const_iterator cit = this->getViewer()->GetProps().cbegin(); 
			cit != this->getViewer()->GetProps().cend(); ++cit){
			ImageSlice* slice = qobject_cast<ImageSlice*>(*cit);
			if (slice) {
				InteractorStyleWindowLevel::pockedImage =  qobject_cast<Image*>(slice->getRenderDataSet());
				return InteractorStyleWindowLevel::pockedImage;
			}
		}
	}
	for (ConnectionList::const_iterator cit = connections.cbegin(); cit != connections.cend(); ++cit)
	{
		disconnect(*cit);
	}
	return InteractorStyleWindowLevel::pockedImage;
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
		vtkInteractorStyleWindowLevel::OnLeftButtonDown();
	}
	else
	{
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}
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
