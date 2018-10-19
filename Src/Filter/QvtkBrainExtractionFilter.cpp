// me 
#include "QvtkBrainExtractionFilter.h"
#include "ui_QvtkBrainExtractionFilter.h"
#include "QvtkScene.h"
#include "QvtkImage.h"
#include "QDebug"
// vtk
#include <vtkObjectFactory.h>
#include <vtkCallbackCommand.h>
#include <vtkSmartPointer.h>
vtkStandardNewMacro(Q::vtk::BrainExtractionFilter)
void Q::vtk::BrainExtractionFilter::PrintSelf(ostream & os, vtkIndent indent)
{
	Superclass::PrintSelf(os, indent);
}
Q::vtk::BrainExtractionFilter::BrainExtractionFilter(QWidget * parent):
	Filter2(parent)
{
	this->setupUi(this);
}

Q::vtk::BrainExtractionFilter::~BrainExtractionFilter()
{
}

static void callback(vtkObject *caller, unsigned long eid,
	void *clientdata, void *calldata) {
	Q::vtk::BrainExtractionFilter *self = reinterpret_cast<Q::vtk::BrainExtractionFilter*>(caller);
	cerr << "Progress: " << self->GetProgress() << "\n";
}

void Q::vtk::BrainExtractionFilter::Update()
{
	Scene *scene = Scene::getCurrentScene();
	Image *image = scene->getDataByUniqueName<Image>(this->lineEditInput->text());
	if (!image) {
		qCritical() << "There is not Q::vtk::Image" << image->getUniqueName() << "in current scene.";
		return;
	}
	this->SetInputConnection(image->getTransformOutputPort());
	vtkSmartPointer<vtkCallbackCommand> callbackCommand =
		vtkSmartPointer <vtkCallbackCommand>::New();
	callbackCommand->SetCallback(callback);
	this->AddObserver(vtkCommand::ProgressEvent, callbackCommand);
	Superclass::Update();

}
