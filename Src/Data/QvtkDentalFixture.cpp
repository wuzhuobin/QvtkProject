// me
#include "QvtkDentalFixture.h"


// qt 
#include <QStandardItem>
#include <QCoreApplication>
#include <QDebug>
namespace Q {
	namespace vtk {

Q_VTK_DATA_CPP(DentalFixture);
DentalFixture::DentalFixture()
{
	this->m_dentalFixtureBrand = this->createAttribute(K.DentalFixtureBrand, "", true);
	this->m_dentalFixtureModel = this->createAttribute(K.DentalFixtureModel, "", true);
	this->m_dentalFixtureShape = this->createAttribute(K.DentalFixtureShape, "", true);
	this->m_dentalFixtureLength = this->createAttribute(K.DentalFixtureLength, 0.0, true);
	this->m_dentalFixtureRadius = this->createAttribute(K.DentalFixtureRadius, 0.0, true);

	this->insertSlotFunction(this->m_dentalFixtureBrand, &DentalFixture::setDentalFixtureBrand);
	this->insertSlotFunction(this->m_dentalFixtureModel, &DentalFixture::setDentalFixtureModel);
	this->insertSlotFunction(this->m_dentalFixtureShape, &DentalFixture::setDentalFixtureShape);
	this->insertSlotFunction(this->m_dentalFixtureLength, &DentalFixture::setDentalFixtureLength);
	this->insertSlotFunction(this->m_dentalFixtureRadius, &DentalFixture::setDentalFixtureRadius);

}

DentalFixture::~DentalFixture()
{
}

bool DentalFixture::readData(QString rootDirectory)
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
		path = QCoreApplication::applicationDirPath() + "/DentalFixture/" + this->getRelativePath().first();
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

QString DentalFixture::getDentalFixtureBrand() const
{
	return DentalFixture::getAttribute(this->m_dentalFixtureBrand).toString();
}

QString DentalFixture::getDentalFixtureModel() const
{
	return DentalFixture::getAttribute(this->m_dentalFixtureModel).toString();
}

QString DentalFixture::getDentalFixtureShape() const
{
	return DentalFixture::getAttribute(this->m_dentalFixtureShape).toString();
}

double DentalFixture::getDentalFixtureLength() const
{
	return DentalFixture::getAttribute(this->m_dentalFixtureLength).toDouble();
}

double DentalFixture::getDentalFixtureRadius() const
{
	return DentalFixture::getAttribute(this->m_dentalFixtureRadius).toDouble();
}

void DentalFixture::setDentalFixtureBrand(QString brand)
{
	DentalFixture::setAttribute(this->m_dentalFixtureBrand, brand);
	emit dentalFixtureBrandChanged(brand);
}

void DentalFixture::setDentalFixtureModel(QString model)
{
	DentalFixture::setAttribute(this->m_dentalFixtureModel, model);
	emit dentalFixtureModelChanged(model);
}

void DentalFixture::setDentalFixtureShape(QString shape)
{
	DentalFixture::setAttribute(this->m_dentalFixtureShape, shape);
	emit dentalFixtureShapeChanged(shape);
}

void DentalFixture::setDentalFixtureLength(double length)
{
	DentalFixture::setAttribute(this->m_dentalFixtureLength, length);
	emit dentalFixtureLengthChanged(length);
}

void DentalFixture::setDentalFixtureRadius(double radius)
{
	DentalFixture::setAttribute(this->m_dentalFixtureRadius, radius);
	emit dentalFixtureRadiusChanged(radius);
}

void DentalFixture::setDentalFixtureBrand(Data * data, QStandardItem * item)
{
	DentalFixture* self = static_cast<DentalFixture*>(data);
	self->setDentalFixtureBrand(DentalFixture::getAttribute(item).toString());
}

void DentalFixture::setDentalFixtureModel(Data * data, QStandardItem * item)
{
	DentalFixture* self = static_cast<DentalFixture*>(data);
	self->setDentalFixtureModel(DentalFixture::getAttribute(item).toString());
}

void DentalFixture::setDentalFixtureShape(Data * data, QStandardItem * item)
{
	DentalFixture* self = static_cast<DentalFixture*>(data);
	self->setDentalFixtureShape(DentalFixture::getAttribute(item).toString());
}

void DentalFixture::setDentalFixtureLength(Data * data, QStandardItem * item)
{
	DentalFixture* self = static_cast<DentalFixture*>(data);
	self->setDentalFixtureLength(DentalFixture::getAttribute(item).toDouble());
}

void DentalFixture::setDentalFixtureRadius(Data * data, QStandardItem * item)
{
	DentalFixture* self = static_cast<DentalFixture*>(data);
	self->setDentalFixtureRadius(DentalFixture::getAttribute(item).toDouble());
}

	}
}