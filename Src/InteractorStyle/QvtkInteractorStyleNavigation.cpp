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
#include <vtkImageData.h> 
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
		connect(this->m_viewer, &OrthogonalViewer::cursorPositionChanged,
			this, static_cast<void(InteractorStyleNavigation::*)(double, double, double)>(&InteractorStyleNavigation::SetCursorPosition));
	}
	else {
		disconnect(this->m_viewer, &OrthogonalViewer::cursorPositionChanged,
			this, static_cast<void(InteractorStyleNavigation::*)(double, double, double)>(&InteractorStyleNavigation::SetCursorPosition));
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

bool InteractorStyleNavigation::GetFast()
{
	return ui->checkBoxFast->isChecked();
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
	if (this->ui->checkBoxFast->isChecked()) {
		this->m_viewer->setCursorPosition(x, y, z);
		return;
	}
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
	this->m_viewer->setCursorPosition(x, y, z);
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
	Image *image = qobject_cast<Image*>(InteractorStyleNavigation::pokedDataSet);
	if (image) {
		double xyz[3]{ x, y, z };
		int ijk[3];
		image->worldCoordinateToImageCoordinate(xyz, ijk);
		this->ui->spinBoxI->blockSignals(true);
		this->ui->spinBoxJ->blockSignals(true);
		this->ui->spinBoxK->blockSignals(true);
		this->ui->spinBoxI->setValue(ijk[0]);
		this->ui->spinBoxJ->setValue(ijk[1]);
		this->ui->spinBoxK->setValue(ijk[2]);
		this->ui->spinBoxI->blockSignals(false);
		this->ui->spinBoxJ->blockSignals(false);
		this->ui->spinBoxK->blockSignals(false);
		if (!this->ui->checkBoxFast->isChecked()) {
			const int *extent = image->getImageData()->GetExtent();
			if (extent[0] <= ijk[0] && ijk[0] <= extent[1] &&
				extent[2] <= ijk[1] && ijk[1] <= extent[3] &&
				extent[4] <= ijk[2] && ijk[2] <= extent[5]) {
				double voxelValue = 0;
				int numOfComponents = image->getImageData()->GetNumberOfScalarComponents();
				if (numOfComponents == 1) {
					voxelValue = image->getImageData()->GetScalarComponentAsDouble(ijk[0], ijk[1], ijk[2], 0);
				}
				else {
					for (size_t i = 0; i < numOfComponents; ++i) {
						double _v = image->getImageData()->GetScalarComponentAsDouble(ijk[0], ijk[1], ijk[2], i);
						voxelValue += _v * _v;
					}
					voxelValue = sqrt(voxelValue);
				}
				this->ui->doubleSpinBoxVoxelValue->setValue(voxelValue);
			}
		}
	}
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

void InteractorStyleNavigation::SetFast(bool fast)
{
	//this->fast = fast;
	this->ui->doubleSpinBoxX->blockSignals(fast);
	this->ui->doubleSpinBoxY->blockSignals(fast);
	this->ui->doubleSpinBoxZ->blockSignals(fast);
	this->ui->spinBoxI->blockSignals(fast);
	this->ui->spinBoxJ->blockSignals(fast);
	this->ui->spinBoxK->blockSignals(fast);
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

void InteractorStyleNavigation::SetCursorPositionI(int i)
{
	int j = this->ui->spinBoxJ->value();
	int k = this->ui->spinBoxK->value();
	this->SetCursorPosition(i, j, k);
}

void InteractorStyleNavigation::SetCursorPositionJ(int j)
{
	int i = this->ui->spinBoxI->value();
	int k = this->ui->spinBoxK->value();
	this->SetCursorPosition(i, j, k);
}

void InteractorStyleNavigation::SetCursorPositionK(int k)
{
	int i = this->ui->spinBoxI->value();
	int j = this->ui->spinBoxJ->value();
	this->SetCursorPosition(i, j, k);
}

void InteractorStyleNavigation::SetCursorPosition(int i, int j, int k)
{
	int ijk[3]{ i,j,k };
	Image *image = qobject_cast<Image*>(InteractorStyleNavigation::pokedDataSet);
	if (image) {
		double xyz[3];
		image->imageCoordinateToWorldCoordinate(ijk, xyz);
		this->SetCursorPosition(xyz);
	}
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
}

InteractorStyleNavigation::InteractorStyleNavigation()
{
	//this->m_trackBallCamera = false;
	this->VolumePicker = vtkVolumePicker::New();
	this->pickedPos[0] = 0.0;
	this->pickedPos[1] = 0.0;
	this->pickedPos[2] = 0.0;
}

InteractorStyleNavigation::~InteractorStyleNavigation()
{
	this->VolumePicker->Delete();
}

bool InteractorStyleNavigation::CalculateIndex(double index[3])
{
	DataSet *dataSet = this->findPokedDataSet();
	memcpy(index, this->pickedPos, sizeof(double) * 3);
	return dataSet != nullptr;
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
	connect(this->ui->spinBoxI, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, &InteractorStyleNavigation::SetCursorPositionI);
	connect(this->ui->spinBoxJ, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, &InteractorStyleNavigation::SetCursorPositionJ);
	connect(this->ui->spinBoxK, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, &InteractorStyleNavigation::SetCursorPositionK);
	connect(this->ui->checkBoxFast, &QCheckBox::stateChanged, this, &InteractorStyleNavigation::SetFast);
	this->ui->doubleSpinBoxX->setRange(VTK_DOUBLE_MIN, VTK_DOUBLE_MAX);
	this->ui->doubleSpinBoxY->setRange(VTK_DOUBLE_MIN, VTK_DOUBLE_MAX);
	this->ui->doubleSpinBoxZ->setRange(VTK_DOUBLE_MIN, VTK_DOUBLE_MAX);
	this->ui->spinBoxI->setRange(VTK_INT_MIN, VTK_INT_MAX);
	this->ui->spinBoxJ->setRange(VTK_INT_MIN, VTK_INT_MAX);
	this->ui->spinBoxK->setRange(VTK_INT_MIN, VTK_INT_MAX);
	this->ui->doubleSpinBoxVoxelValue->setRange(VTK_DOUBLE_MIN, VTK_DOUBLE_MAX);
}
int InteractorStyleNavigation::tryPick(const int XY[2])
{
	int picked = 0;
	int _XY[2];
	if (XY == nullptr) {
		this->GetInteractor()->GetEventPosition(_XY);
	}
	else {
		memcpy(_XY, XY, sizeof(_XY));
	}
	vtkAbstractPicker *picker = this->GetInteractor()->GetPicker();
	//Pick
	picked = picker->Pick(
		_XY[0],
		_XY[1],
		0,  // always zero.
		this->GetCurrentRenderer());
	if (picked == 0) {
		picker = this->VolumePicker;
		picked = picker->Pick(
			_XY[0],
			_XY[1],
			0,  // always zero.
			this->GetCurrentRenderer());
	}
	picker->GetPickPosition(this->pickedPos);
	return picked != 0;
}

}
}
