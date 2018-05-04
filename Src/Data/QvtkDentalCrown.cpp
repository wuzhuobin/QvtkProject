// me
#include "QvtKDentalCrown.h"
#include "QvtkProp.h"

#include <QDebug>
#include <QCoreApplication>
namespace Q {
	namespace vtk {

Q_VTK_DATA_CPP(DentalCrown);
DentalCrown::DentalCrown()
{
	//this->setRelativePath(QStringList() << "DentalCrown/Crown_11.stl");
	unsigned int id = this->getToothPosition().toFDI();
	this->setRelativePath(QStringList() << "Crown_" + QString::number(id) + ".stl");
	this->readData("...");
}

bool DentalCrown::readData(QString rootDirectory)
{
	QString path;
	if (rootDirectory.isEmpty()) {
		return AnnotationPolyData::readData(rootDirectory);
	}
	else {
		if (this->getRelativePath().isEmpty() ||
			this->getRelativePath().first().isEmpty()) {
			qCritical() << "getRelativePath() is empty. ";
			return false;
		}
		path = QCoreApplication::applicationDirPath() + "/DentalCrown/" + this->getRelativePath().first();
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

void DentalCrown::setToothPosition(ToothPosition toothPosition)
{

	if (this->getToothPosition().toFDI() == toothPosition.toFDI()) {
		return;
	}
	this->DentalImplant::setToothPosition(toothPosition);
	unsigned int id = this->getToothPosition().toFDI();
	this->setRelativePath(QStringList() << "Crown_" + QString::number(id) + ".stl");
	this->readData("...");
	for each (Prop* prop in *this->getReferenceProps())
	{
		prop->resetDisplayRegion();
	}
}

	}
}