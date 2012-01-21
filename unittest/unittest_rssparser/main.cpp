#include <QtCore/QCoreApplication>
#include <QtTest/QtTest>
#include "test_rssparser.h"

#define USE_TESTLIB
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    test_rssparser test;

#ifdef USE_TESTLIB
    QTest::qExec(&test,argc,argv);
    return a.exec();
#else
    test.endtoendtest();
    return a.exec();
#endif

}
