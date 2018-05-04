// me 
#include "QvtkPatientInfo.h"
//qt
#include <QDomDocument>

namespace Q {
	namespace vtk {

		Q_VTK_DATA_CPP(PatientInfo)
		PatientInfo::PatientInfo()
		{

		}

		PatientInfo::~PatientInfo()
		{

		}

		void PatientInfo::setPatientID(int id)
		{
			this->m_patientID = id;
		}


		void PatientInfo::setName(QString name)
		{
			this->m_name = name;
		}


		void PatientInfo::setAge(int age)
		{
			this->m_age = age;
		}


		void PatientInfo::setGender(int gender)
		{
			this->m_gender = gender;
		}

		int PatientInfo::getPatientID() const
		{
			return this->m_patientID;
		}

		QString PatientInfo::getName() const
		{
			return this->m_name;
		}

		int PatientInfo::getAge() const
		{
			return this->m_age;
		}

		int PatientInfo::getGender() const
		{
			return this->m_gender;
		}

		void PatientInfo::writeXML(QDomElement & xml, QString directoryPath) const
		{
			Data::writeXML(xml, directoryPath);
			xml.setAttribute(K.PatientID, this->getPatientID());
			xml.setAttribute(K.Name, this->getName());
			xml.setAttribute(K.Age, this->getAge());
			xml.setAttribute(K.Gender, this->getGender());
		}

		void PatientInfo::readXML(const QDomElement& xml, QString directoryPath)
		{
			Data::readXML(xml, directoryPath);
			this->setPatientID(xml.attribute(K.PatientID).toInt());
			this->setName(xml.attribute(K.Name));
			this->setAge(xml.attribute(K.Age).toInt());
			this->setGender(xml.attribute(K.Gender).toInt());
		}
	}
}

