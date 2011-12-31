#ifndef TEST_RSSPARSER_H
#define TEST_RSSPARSER_H

#include <QObject>
#include "rssparser.h"

class QBuffer;
class test_rssparser : public QObject
{
    Q_OBJECT

public:
    test_rssparser();
    void endtoendtest();

private slots:

    void initTestCase();
    void test_setSource();
    void test_channelElement_title();
    void test_channelElement_link();
    void test_channelElement_desc();
    void test_feedmanager_getintresteditems();
    void test_allitemelements();
    void test_category();
    void test_categories();
    void cleanupTestCase();

private:
    RSSParser* mRSSParser;
    QBuffer* mBuffer;

    // This is for feedmanager
    QList<RSSParser::RSSElement> mIntrestedItemElements;
    int mUpdatedCount;
};

#endif // TEST_RSSPARSER_H
