#ifndef __SCB_PATIENT_INFO_H__
#define __SCB_PATIENT_INFO_H__
#pragma once
// me 
#include "data_export.h"
#include "QvtkData.h"
namespace Q {
	namespace vtk {

		class PatientInfo : public Data
		{
			Q_OBJECT;
			Q_VTK_DATA_H(
				PatientInfo,
				Q_VTK_KEY(PatientID)
				Q_VTK_KEY(Name)
				Q_VTK_KEY(Age)
				Q_VTK_KEY(Gender)
			);

		public:
			PatientInfo();
			virtual ~PatientInfo() override;

			virtual int getPatientID() const;
			virtual QString getName() const;
			virtual int getAge() const;
			virtual int getGender() const;

			virtual void readXML(const QDomElement& xml, QString directoryPath = QString()) override;
			virtual void writeXML(QDomElement& xml, QString directoryPath = QString()) const override;

			public slots:
			virtual void setPatientID(int id);
			virtual void setName(QString name);
			virtual void setAge(int age);
			virtual void setGender(int gender);

		protected:
			virtual Data* newInstance() const override { return new PatientInfo; }

		private:
			int m_patientID;
			QString  m_name;
			int m_age;
			int m_gender;

		};

	}
}


#endif // !__SCB_PATIENT_INFO_H__
