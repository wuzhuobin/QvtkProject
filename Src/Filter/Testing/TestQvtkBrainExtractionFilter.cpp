// qt
#include <QTest>
// me
#include "QvtkBrainExtractionFilter.h"
class TestQvtkBrainExtractionFilter : public QObject
{
	Q_OBJECT;
private Q_SLOTS:
	void initTestCase(){}
	void cleanupTestCase() {}
	void init(){}
	void cleanup() {}
	void test()
	{ 
		Q::vtk::BrainExtractionFilter f;
	}
};
QTEST_MAIN(TestQvtkBrainExtractionFilter)
#include "TestQvtkBrainExtractionFilter.moc"