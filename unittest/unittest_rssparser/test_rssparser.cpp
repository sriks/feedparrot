#include "test_rssparser.h"
#include <QFile>
#include <QBuffer>
#include <QDebug>
#include <QtTest/QtTest>
#include <QList>
#include <QLabel>

test_rssparser::test_rssparser()
{
    initTestCase();
}

void test_rssparser::initTestCase()
{
    mRSSParser = new RSSParser(this);
    mBuffer = new QBuffer(this);
    mIntrestedItemElements<<RSSParser::title<<RSSParser::link<<RSSParser::description<<RSSParser::author;
    mUpdatedCount = 4;
}

void test_rssparser::cleanupTestCase()
{

}

void test_rssparser::test_setSource()
{
    QFile sourceFile(":/feed/testfeed.htm");
    if(sourceFile.open(QIODevice::ReadOnly))
    {

        // Read content to buffer
        mBuffer->setData(sourceFile.readAll());
        // Open buffer in read mode
        qDebug()<<"opening buffer "<<mBuffer->open(QIODevice::ReadOnly);
        // Set source to rssparser
    mRSSParser->setSource(mBuffer);
    }
    else
    {
    qDebug()<<"failed to open source feed file";
    }
    sourceFile.close();
}

void test_rssparser::test_channelElement_title()
{
    QString result;
    QBENCHMARK
    {
        result = mRSSParser->channelElement(RSSParser::title);
    }
    qDebug()<<result;
}

void test_rssparser::test_channelElement_link()
{
    QString result;
    QBENCHMARK
    {
        result = mRSSParser->channelElement(RSSParser::link);
    }
    qDebug()<<result;
}

void test_rssparser::test_channelElement_desc()
{
    QString result;
    QBENCHMARK
    {
        result = mRSSParser->channelElement(RSSParser::description);
    }
    qDebug()<<result;
}

void test_rssparser::test_feedmanager_getintresteditems()
{
    QList<QStringList> resultList;
    int intrestedElementCount = mIntrestedItemElements.count();
    for(int i=0;i<mUpdatedCount;i++)
    {
        QStringList resultStrings;
        for(int j=0;j<intrestedElementCount;j++)
        {
        QString result = mRSSParser->itemElement(i,mIntrestedItemElements.at(j));
        resultStrings.append(result);
        qDebug()<<result;
        }
        resultList.append(resultStrings);
    }

    for(int i = 0;i<resultList.count();i++)
    {
        qDebug()<<resultList.at(i);
    }
}

void test_rssparser::test_category()
{
    QStringList list = mRSSParser->category(1);
    qDebug()<<mRSSParser->itemElement(1,RSSParser::title);
    qDebug()<<"Count:"<<list.count();
    qDebug()<<list;

}

void test_rssparser::test_categories()
{
    QList<QStringList> list;
    QBENCHMARK_ONCE{
         list = mRSSParser->categories();}
    qDebug()<<"categories count:"<<list.count();
    qDebug()<<list;
}

void test_rssparser::test_allitemelements()
{
    QStringList result;
    QBENCHMARK_ONCE{
    result = mRSSParser->itemElements(RSSParser::title);}
    qDebug()<<result.count();
}

// eof
