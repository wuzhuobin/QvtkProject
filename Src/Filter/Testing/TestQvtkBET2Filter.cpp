// qt
#include <QTest>
// me
#include "QvtkBET2Filter.h"
class TestQvtkBET2Filter : public QObject
{
	Q_OBJECT;
private Q_SLOTS:
	void initTestCase(){}
	void cleanupTestCase() {}
	void init(){}
	void cleanup() {}
	void test()
	{ 
		TestQvtkBET2Filter f;
	}
};
QTEST_MAIN(QvtkBET2Filter)
#include "TestQvtkBET2Filter.moc"