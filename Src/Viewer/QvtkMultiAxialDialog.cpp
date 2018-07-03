// me
#include "QvtkMultiAxialDialog.h"
#include "ui_QvtkMultiAxialDialog.h"
#include "QvtkPlanarViewer.h"
#include "ui_QvtkOrthogonalViewer.h"
#include "QvtkScene.h"
#include "QvtkImage.h"
#include "QvtkImageSlice.h"
// test
#include "QvtkPolyData.h"
#include <vtkActor.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyData.h>
// vtk
#include <vtkRenderWindow.h>
#include <vtkTransform.h>
// qt 
#include <QDebug>
Q::vtk::MultiAxialDialog::MultiAxialDialog(unsigned int row, unsigned col, QWidget *parent) :
	row(row),
	col(col),
    QDialog(parent),
    ui(new Ui::MultiAxialDialog)
{
    this->ui->setupUi(this);
	for (unsigned int i = 0; i < this->row; ++i) {
		for (unsigned int j = 0; j < this->col; ++j) {
			PlanarViewer *viewer = new PlanarViewer(this);
			this->viewers.push_back(viewer);
			QList<QPushButton*> pushButtons = viewer->findChildren<QPushButton*>();
			for (QPushButton *button : pushButtons) {
				button->setVisible(false);
			}
			this->ui->gridLayout->addWidget(viewer, i, j);
		}
	}
	// connection
	QObject::connect(this->ui->pushButtonUpdate, &QPushButton::clicked,
		this, &MultiAxialDialog::updateAxialViewer);
}

Q::vtk::MultiAxialDialog::MultiAxialDialog(QWidget *parent) :
	MultiAxialDialog(5, 2, parent)
{

}

Q::vtk::MultiAxialDialog::~MultiAxialDialog()
{
	for (PlanarViewer *viewer : this->viewers) {
		QList<Prop*> props = viewer->getProps();
		viewer->removeAllProp();
		for (Prop *prop : props) {
			delete prop;
		}
	}
    delete ui;
}

Q::vtk::PlanarViewer * Q::vtk::MultiAxialDialog::getViewer(unsigned int row, unsigned int col)
{
	int index = row * this->col + col;
	if (this->viewers.size() <= index) {
		return nullptr;
	}
	return this->viewers[index];
}

void Q::vtk::MultiAxialDialog::updateAxialViewer()
{
	Scene *scene = Scene::getCurrentScene();
	Image *image = scene->getDataByUniqueName<Image>("Image");
	PolyData *biopsy = scene->getDataByUniqueName<PolyData>("Biopsy tube");
	vtkPolyData *polyData = biopsy->getPolyData();
	double normal[3];
	double point0[3], point1[3];
	polyData->GetPoint(0, point0);
	polyData->GetPoint(1, point1);
	vtkMath::Subtract(point0, point1, normal);
	int i = -5;
	for (PlanarViewer *viewer : this->viewers) {
		if (viewer->getProps().isEmpty()) {
			viewer->addProp(new ImageSlice);
		}
		Prop* prop = viewer->getProps().front();
		prop->setRenderDataSet(image);
		viewer->setAxialViewPlaneNormal(normal);
		viewer->setOrientationToAxial();
		viewer->setEnableCornerAnnotation(true);
		viewer->setSliceThickness(3);
		viewer->resetCamera(0);
		viewer->resetCameraClippingRange(0);
		viewer->getRenderWindow()->Render();
		viewer->setCursorTransformFunction([i](double *pos, vtkMatrix4x4* cursorMatrix, Viewer* viewer) {
			PlanarViewer *planaViewer = static_cast<PlanarViewer*>(viewer);
			const double *normal = planaViewer->getCurrentPlaneNormal();
			double translation[3];
			std::copy(normal, normal + 3, translation);
			vtkMath::MultiplyScalar(translation, i * planaViewer->getSliceThickness());
			vtkMath::Add(pos, translation, pos);
			if (cursorMatrix == nullptr) {
				viewer->getCursorActor()->SetUserMatrix(vtkSmartPointer<vtkMatrix4x4>::New());
				cursorMatrix = viewer->getCursorActor()->GetUserMatrix();
			}
			cursorMatrix->SetElement(0, 3, translation[0]);
			cursorMatrix->SetElement(1, 3, translation[1]);
			cursorMatrix->SetElement(2, 3, translation[2]);
		});
		//ImageSlice *slice = static_cast<ImageSlice*>(viewer->getProps().first());
		++i;
	}
}
