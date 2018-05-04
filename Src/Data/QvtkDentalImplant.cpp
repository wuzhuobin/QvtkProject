// me
#include "QvtkDentalImplant.h"


// qt 
#include <QDebug>
#include <QDomElement>
#include <QStandardItem>

namespace Q {
	namespace vtk {
Q_VTK_DATA_CPP(DentalImplant);
DentalImplant::DentalImplant()
{
	this->setColor(1, 0, 0);
	this->setHoverColor(0, 1, 0);
	this->setSelectedColor(0, 0, 1);
}

DentalImplant::~DentalImplant()
{
}

void DentalImplant::printSelf() const
{
	AnnotationPolyData::printSelf();
}

void DentalImplant::readXML(const QDomElement & xml, QString directoryPath)
{
	AnnotationPolyData::readXML(xml, directoryPath);
	ToothPosition toothPos;
	toothPos.orientation = static_cast<bool>(xml.attribute(K.ToothPositionOrientation).toUShort());
	toothPos.side = static_cast<bool>(xml.attribute(K.ToothPositionSide).toUShort());
	toothPos.index = xml.attribute(K.ToothPositionIndex).toUShort();
	this->setToothPosition(toothPos);
}

void DentalImplant::writeXML(QDomElement & xml, QString directoryPath) const
{
	AnnotationPolyData::writeXML(xml, directoryPath);
	xml.setAttribute(K.ToothPositionSide, this->getToothPosition().side);
	xml.setAttribute(K.ToothPositionIndex, this->getToothPosition().index);
	xml.setAttribute(K.ToothPositionOrientation, this->getToothPosition().orientation);
}

void DentalImplant::setToothPosition(DentalImplant::ToothPosition toolPosition)
{
	this->m_toothPosition = toolPosition;
	emit this->m_toothPosition;
}

unsigned int DentalImplant::ToothPosition::toFDI() const
{
	unsigned int FDI;
	if (this->orientation == UPPER && this->side == RIGHT) {
		FDI = 10;
	}
	else if (this->orientation == UPPER && this->side == LEFT) {
		FDI = 20;
	}
	else if (this->orientation == LOWER && this->side == RIGHT) {
		FDI = 40;
	}
	else {
		FDI = 30;
	}
	return FDI = FDI + this->index + 1;
}

void DentalImplant::ToothPosition::fromFDI(unsigned int i)
{
	if (11 <= i && i <= 17) {
		i -= 11;
		this->orientation = UPPER;
		this->side = RIGHT;
	}
	else if (21 <= i && i <= 27) {
		this->orientation = UPPER;
		this->side = LEFT;
		i -= 21;
	}
	else if (31 <= i && i <= 37) {
		this->orientation = LOWER;
		this->side = LEFT;
		i -= 31;
	}
	else if (41 <= i && i <= 47) {
		this->orientation = LOWER;
		this->side = RIGHT;
		i -= 41;
	}
	else {
		qCritical() << "Input i =" << i << "not following dental numbering system. ";
		qCritical() << "Please make sure it is 11-17 ,21-27, 31-37, 41-47. ";
		return;
	}
		this->index = i;
}
	}
}