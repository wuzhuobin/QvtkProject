// me 
#include "vtkBrokenLineSource.h"
// qt 
#include <QtTest>
#include <QDebug>
// vtk 
#include <vtkSmartPointer.h>
class TestvtkBrokenLineSource: public QObject
{
	Q_OBJECT;
private Q_SLOTS:
	void initTestCase() {
		this->lineSource = nullptr;
	}
	void cleanupTestCase() {
		this->lineSource = nullptr;
	}
	void init() {
		this->lineSource = vtkSmartPointer<vtkBrokenLineSource>::New();
	}
	void cleanup() {
		this->lineSource = nullptr;
	}
    void toUpper();
	void smartPointerCreation();
	void nonBrokenCheck();
private:
	vtkSmartPointer<vtkBrokenLineSource> lineSource;
};

void TestvtkBrokenLineSource::smartPointerCreation()
{
	vtkSmartPointer<vtkBrokenLineSource> l =
		vtkSmartPointer<vtkBrokenLineSource>::New();
}

void TestvtkBrokenLineSource::nonBrokenCheck()
{
	//double 
	//this->lineSource->SetPoint1()
}

void TestvtkBrokenLineSource::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

QTEST_MAIN(TestvtkBrokenLineSource)
#include "TestvtkBrokenLineSource.moc"

