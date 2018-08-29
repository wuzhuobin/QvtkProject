// me 
#include "QvtkBET2Filter.h"
#include "ui_QvtkBET2Filter.h"
#include "QvtkScene.h"
#include "QvtkImage.h"
#include "QDebug"
// vtk
#include <vtkObjectFactory.h>
vtkStandardNewMacro(Q::vtk::BET2Filter)
void Q::vtk::BET2Filter::PrintSelf(ostream & os, vtkIndent indent)
{
	os << *this;
	Superclass::PrintSelf(os, indent);
}
Q::vtk::BET2Filter::BET2Filter(QWidget * parent):
	Filter2(parent)
{
	this->setupUi(this);
}

Q::vtk::BET2Filter::~BET2Filter()
{
}

void Q::vtk::BET2Filter::Update()
{
	Scene *scene = Scene::getCurrentScene();
	Image *image = scene->getDataByUniqueName<Image>(this->lineEditInput->text());
	if (!image) {
		qCritical() << "There is not Q::vtk::Image" << image->getUniqueName() << "in current scene.";
		return;
	}
	this->SetInputConnection(image->getTransformOutputPort());
	Superclass::Update();
}
