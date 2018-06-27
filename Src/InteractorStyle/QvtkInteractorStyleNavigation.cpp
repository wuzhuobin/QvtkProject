// me
#include "QvtkInteractorStyleNavigation.h"
#include "ui_QvtkInteractorStyleNavigation.h"
#include "QvtkScene.h"
#include "QvtkImage.h"
#include "QvtkPlanarProp.h"
#include "QvtkOrthogonalViewer.h"
#include "QvtkPlanarViewer.h"
#include "QvtkVolume.h"

// vtk
#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolumePicker.h>
#include <vtkPropPicker.h>
#include <vtkProp3D.h>
#include <vtkSmartPointer.h>

#include <vtkCallbackCommand.h>
// qt
#include <QDebug>
namespace Q {
	namespace vtk {

vtkStandardNewMacro(InteractorStyleNavigation);
void InteractorStyleNavigation::PrintSelf(ostream & os, vtkIndent indent)
{
	vtkInteractorStyleNavigation::PrintSelf(os, indent);
}

void InteractorStyleNavigation::setCustomEnable(bool flag)
{
	UniqueUiInteractorObserver::setCustomEnable(flag);
	if (flag) {
		connect(this->m_viewer, &OrthogonalViewer::CursorPositionChanged,
			this, &InteractorStyleNavigation::SetCursorPosition);
	}
	else {
		disconnect(this->m_viewer, &OrthogonalViewer::CursorPositionChanged,
			this, &InteractorStyleNavigation::SetCursorPosition);
	}
}

void InteractorStyleNavigation::install()
{
	UNIQUE_UI_INSTALL(InteractorStyleNavigation);
}

void InteractorStyleNavigation::uninstall()
{
	UNIQUE_UI_UNINSTALL();
}

void InteractorStyleNavigation::OnLeftButtonDown()
{
	vtkInteractorStyleNavigation::OnLeftButtonDown();
	switch (this->State)
	{
	case VTKIS_NAVIGATION:
		this->Navigation();
		break;
	default:
		break;
	}
	// Lowering update rate. 
	FunctionSet2 f = [](InteractorObserver *ob) {
		static_cast<InteractorStyleNavigation*>(ob)->StartInteraction();
	};
	synchronalCall(this, f);
}

void InteractorStyleNavigation::OnLeftButtonUp()
{
	vtkInteractorStyleNavigation::OnLeftButtonUp();
	// Restoring update rate.
	FunctionSet2 f = [](InteractorObserver *ob) {
		InteractorStyleNavigation *style = static_cast<InteractorStyleNavigation*>(ob);
		style->EndInteraction();
		style->GetInteractor()->Render();
	};
	synchronalCall(this, f);
}

void InteractorStyleNavigation::SetCursorPosition(double x, double y, double z)
{
	if (this->ui->doubleSpinBoxX->value() == x &&
		this->ui->doubleSpinBoxY->value() == y &&
		this->ui->doubleSpinBoxZ->value() == z) {
		return;
	}
	// connect and disconnect to speed up
	//disconnect(this->m_ui->doubleSpinBoxX, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
	//	static_cast<InteractorStyleNavigation*>(getUniqueThis()), &InteractorStyleNavigation::SetCursorPositionX);
	//disconnect(this->m_ui->doubleSpinBoxY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
	//	static_cast<InteractorStyleNavigation*>(getUniqueThis()), &InteractorStyleNavigation::SetCursorPositionY);
	//disconnect(this->m_ui->doubleSpinBoxZ, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
	//	static_cast<InteractorStyleNavigation*>(getUniqueThis()), &InteractorStyleNavigation::SetCursorPositionZ);
	this->ui->doubleSpinBoxX->blockSignals(true);
	this->ui->doubleSpinBoxY->blockSignals(true);
	this->ui->doubleSpinBoxZ->blockSignals(true);
	this->m_viewer->SetCursorPosition(x, y, z);

	//double origin[3] = { x, y, z };
	//Scene* scene = Scene::getCurrentScene();
	//QStringList planarProps = scene->getAllDataByClassName("SCBAbstractPlanarProp");

	//for (int i = 0; i < planarProps.size(); ++i) {
	//	SCBAbstractPlanarProp* planarProp = static_cast<SCBAbstractPlanarProp*>(
	//		scene->getDataByUniqueName(planarProps[i]));
	//	if (planarProp->getRendering()) {
	//		planarProp->setPlanarOrigin(origin);
	//	}
	//}

	//const QList<Prop*>* props = this->Image->getReferenceProps();
	//foreach(Prop* prop, *props) {
	//	SCBAbstractPlanarProp* slice = qobject_cast<SCBAbstractPlanarProp*>(prop);
	//	if (slice && slice->getRendering()) {
	//		double origin[3];
	//		origin[0] = x;
	//		origin[1] = y;
	//		origin[2] = z;
	//		//slice->getOrigin(origin);

	//		//SCBImageSlice::ENUM_ORIENTATION orientation =
	//		//	static_cast<SCBImageSlice::ENUM_ORIENTATION>(slice->getOrientation());
	//		//switch (orientation)
	//		//{
	//		//case SCBImageSlice::ORIENTATION_YZ:
	//		//	//if (region[0] == x) {
	//		//	//	return;
	//		//	//}
	//		//	origin[0] = x;
	//		//	break;
	//		//case SCBImageSlice::ORIENTATION_XZ:
	//		//	//if (region[2] == y) {
	//		//	//	return;
	//		//	//}
	//		//	origin[1] = y;
	//		//	break;
	//		//case SCBImageSlice::ORIENTATION_XY:
	//		//	//if (region[4] == z) {
	//		//	//	return;
	//		//	//}
	//		//	origin[2] = z;
	//		//	break;
	//		//case SCBImageSlice::ORIENTATION_OBLIQUE:
	//		//	break;
	//		//default:
	//		//	break;
	//		//}
	//		slice->setPlanarOrigin(origin);
	//	}
	//}

	//if (x < this->ui->doubleSpinBoxX->minimum()) {
	//	this->ui->doubleSpinBoxX->setMinimum(x);
	//}
	//if (x > this->ui->doubleSpinBoxX->maximum()) {
	//	this->ui->doubleSpinBoxX->setMaximum(x);
	//}
	this->ui->doubleSpinBoxX->setValue(x);
	//if (y > this->ui->doubleSpinBoxY->maximum()) {
	//	this->ui->doubleSpinBoxY->setMaximum(y);
	//}
	//if (y < this->ui->doubleSpinBoxY->minimum()) {
	//	this->ui->doubleSpinBoxY->setMinimum(y);
	//}
	this->ui->doubleSpinBoxY->setValue(y);
	//if (z > this->ui->doubleSpinBoxZ->maximum()) {
	//	this->ui->doubleSpinBoxZ->setMaximum(z);
	//}
	//if (z < this->ui->doubleSpinBoxZ->minimum()) {
	//	this->ui->doubleSpinBoxZ->setMinimum(z);
	//}
	this->ui->doubleSpinBoxZ->setValue(z);
	//Image *image = qobject_cast<Image*>(this->findPokedDataSet);
	//if (image) {
	//	
	//}
	//this->ui->spinBoxI->
	// connect and disconnect to speed up
	//connect(this->m_ui->doubleSpinBoxX, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
	//	static_cast<InteractorStyleNavigation*>(getUniqueThis()), &InteractorStyleNavigation::SetCursorPositionX);
	//connect(this->m_ui->doubleSpinBoxY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
	//	static_cast<InteractorStyleNavigation*>(getUniqueThis()), &InteractorStyleNavigation::SetCursorPositionY);
	//connect(this->m_ui->doubleSpinBoxZ, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
	//	static_cast<InteractorStyleNavigation*>(getUniqueThis()), &InteractorStyleNavigation::SetCursorPositionZ);
	this->ui->doubleSpinBoxX->blockSignals(false);
	this->ui->doubleSpinBoxY->blockSignals(false);
	this->ui->doubleSpinBoxZ->blockSignals(false);

}

void InteractorStyleNavigation::SetCursorPositionX(double x)
{
	const double* pos = this->m_viewer->getCursorPosition();
	//if (pos[0] == x) {
	//	return;
	//}
	SetCursorPosition(x, pos[1], pos[2]);
}

void InteractorStyleNavigation::SetCursorPositionY(double y)
{
	const double* pos = this->m_viewer->getCursorPosition();
	//if (pos[1] == y) {
	//	return;
	//}
	SetCursorPosition(pos[0], y, pos[2]);
}

void InteractorStyleNavigation::SetCursorPositionZ(double z)
{
	const double* pos = this->m_viewer->getCursorPosition();
	//if (pos[2] == z) {
	//	return;
	//}
	SetCursorPosition(pos[0], pos[1], z);
}

void InteractorStyleNavigation::SetCursorPositionI(double i)
{
	qWarning() << "Not implementd.";
}

void InteractorStyleNavigation::SetCursorPositionJ(double j)
{
	qWarning() << "Not implementd.";
}

void InteractorStyleNavigation::SetCursorPositionK(double k)
{
	qWarning() << "Not implementd.";
}

void InteractorStyleNavigation::CentralizeCursorPosition()
{
	//if (!GetImage()) {
	//	vtkWarningMacro(<< "Image CBCT is nullptr");
	//	return;
	//}
	double bounds[6];
	QStringList allImage = Scene::getCurrentScene()->getAllDataByClassName("Q::vtk::Image");
	if (allImage.size() == 0) {
		return;
	}
	// if more than one image, use the first image.
	Scene::getCurrentScene()->getDataByUniqueName<Image>(allImage.first())->getWorldBounds(bounds);
	SetCursorPosition(
		(bounds[1] + bounds[0]) *0.5,
		(bounds[3] + bounds[2]) *0.5,
		(bounds[5] + bounds[4]) *0.5
	);
}

void InteractorStyleNavigation::SetCameraScale(double scale)
{
	QList<InteractorObserver*> list = observersDataBase.values(this->metaObject()->className());

	FunctionSet2 functionSet =
		[&scale](InteractorObserver* ib) {
		InteractorStyleNavigation* style = static_cast<InteractorStyleNavigation*>(ib);
		style->SetCurrentRenderer(style->GetDefaultRenderer());
		style->vtkInteractorStyleNavigation::SetCameraScale(scale);
	};
	this->synchronalCall(this, functionSet);
	//int i = 0;
	//SYNCHRONAL_CALL(
	//	InteractorStyleNavigation,
	//	style->SetCurrentRenderer(style->GetDefaultRenderer());
	//	style->vtkInteractorStyleNavigation::SetCameraScale(scale);
	//	++i;
	//	)
	//	qDebug() << "number of i : " << i;

}

InteractorStyleNavigation::InteractorStyleNavigation()
{
	//this->m_trackBallCamera = false;
	this->VolumePicker = vtkVolumePicker::New();
}

InteractorStyleNavigation::~InteractorStyleNavigation()
{
	this->VolumePicker->Delete();
}

bool InteractorStyleNavigation::CalculateIndex(double index[3])
{
	if (vtkInteractorStyleNavigation::CalculateIndex(index)) {
		return true;
	}
	if (!this->VolumePicker->Pick(
		this->GetInteractor()->GetEventPosition()[0],
		this->GetInteractor()->GetEventPosition()[1],
		0,  // always zero.
		this->GetCurrentRenderer())) {
		return false;
	}
	this->VolumePicker->GetPickPosition(index);
	return true;
}

void InteractorStyleNavigation::uniqueInstall()
{
	// here this == uniqueThis
	connect(this->ui->doubleSpinBoxX, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &InteractorStyleNavigation::SetCursorPositionX);
	connect(this->ui->doubleSpinBoxY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &InteractorStyleNavigation::SetCursorPositionY);
	connect(this->ui->doubleSpinBoxZ, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &InteractorStyleNavigation::SetCursorPositionZ);
	this->ui->doubleSpinBoxX->setRange(VTK_DOUBLE_MIN, VTK_DOUBLE_MAX);
	this->ui->doubleSpinBoxY->setRange(VTK_DOUBLE_MIN, VTK_DOUBLE_MAX);
	this->ui->doubleSpinBoxZ->setRange(VTK_DOUBLE_MIN, VTK_DOUBLE_MAX);
}
int InteractorStyleNavigation::tryPick(double xyz[3])
{
	return this->CalculateIndex(xyz);
	//return 0;
}
//int InteractorStyleNavigation::tryPick(double xyz[3])
//{
//	this->SetCurrentRenderer(this->GetDefaultRenderer());
//	int ret = this->GetInteractor()->GetPicker()->Pick(
//		this->GetInteractor()->GetEventPosition()[0],
//		this->GetInteractor()->GetEventPosition()[1],
//		0,  // always zero.
//		this->GetCurrentRenderer());
//	if (!ret) {
//		return this->VolumePicker->Pick(
//			this->GetInteractor()->GetEventPosition()[0],
//			this->GetInteractor()->GetEventPosition()[1],
//			0,  // always zero.
//			this->GetCurrentRenderer());
//	}
//	else {
//		return ret;
//	}
//}
	}
}
