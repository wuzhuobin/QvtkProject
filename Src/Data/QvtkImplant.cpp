// me
#include "QvtkImplant.h"
#include "QvtkProp.h"

// vtk 
#include <vtkSTLReader.h>
#include <vtkNew.h>
#include <vtkMatrix4x4.h>


// qt 
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
namespace Q {
	namespace vtk {


Q_VTK_DATA_CPP(Implant);
const QStringList IMPLANT_PATHS = {
	"implant/Wital Implant.stl", 
	"implant/OrthopedicNail.stl",
	"implant/OrthopedicNail2.stl"
};

Implant::Implant()
{
	this->m_implantSource = createAttribute(K.ImplantSource, static_cast<unsigned int>(0), true);
	this->insertSlotFunction(this->m_implantSource, &Implant::setImplantSource);
	this->setRelativePath(QStringList() << ( IMPLANT_PATHS[0]));
	this->readData("...");
}

Implant::~Implant()
{

}

void Implant::printSelf() const
{
	AnnotationPolyData::printSelf();
}

bool Implant::readData(QString rootDirectory)
{
	QString path;
	if (rootDirectory.isEmpty()) {
		return AnnotationPolyData::readData(rootDirectory);
	}
	else {
		//QCoreApplication::applicationDirPath() + '/' + IMPLANT_PATHS[this->getImplantSource()];
		if (this->getRelativePath().isEmpty() ||
			this->getRelativePath().first().isEmpty()) {
			qCritical() << "getRelativePath() is empty. ";
			return false;
		}
		path = QCoreApplication::applicationDirPath() + '/' + this->getRelativePath().first();

	}
	if (!this->getDataSet())
	{
		qCritical() << "data is a null ptr. ";
		return false;
	}
	// When the path starts with ":", it is in the QRC
	if (path.startsWith(":"))
	{
		return readQRC(path, this->getPolyData());
	}
	else {
		return readDataSuffix(path, this->getPolyData());
	}
}

void Implant::setImplantSource(unsigned int i)
{
	setAttribute(this->m_implantSource, i);
	if (i < 0 || i >= static_cast<unsigned int>(IMPLANT_PATHS.size())) {
		qWarning() << "Requested implant path does not exist. ";
		qWarning() << "There is only" << IMPLANT_PATHS.size() << "paths";
		qWarning() << "While " << i << "is requested. ";
		return;
	}


	this->setRelativePath(QStringList() << IMPLANT_PATHS[i] );
	this->readData("...");

	for (QList<Prop*>::const_iterator cit = this->getReferenceProps()->cbegin();
		cit != this->getReferenceProps()->cend(); ++cit) {
		double bounds[6];
		this->getWorldBounds(bounds);
		(*cit)->setDisplayRegion(bounds);
	}

}

unsigned int Implant::getImplantSource() const
{
	return getAttribute(this->m_implantSource).toUInt();
}

void Implant::setImplantSource(Data * self, QStandardItem * item)
{
	Implant* _self = static_cast<Implant*>(self);
	_self->setImplantSource(getAttribute(item).toUInt());
}
	}
}
