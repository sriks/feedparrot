#include <QDebug>
#include <QFile>
#include <QDataStream>
#include "test_rssmanager.h"

const QUrl engadgeturl("http://mobile.engadget.com/rss.xml");

test_rssmanager::test_rssmanager(QObject *parent) :
    QObject(parent)
{

}

test_rssmanager::~test_rssmanager()
{
    qDebug()<<__FUNCTION__;
    delete mRSSManager;
}

void test_rssmanager::listAllSubscriptions()
{
    int subCount = mRSSManager->subscriptionCount();
    qDebug()<<"Total subscriptions:"<<subCount;
    QList<FeedSubscription> subList = mRSSManager->subscriptions();
    for(int i=0;i<subList.count();i++)
    {
        qDebug()<<"Source url:"<<subList[i].sourceUrl().toString();
        qDebug()<<"Update interval:"<<subList[i].updateInterval();
    }
}

void test_rssmanager::initTestCase()
{
    mRSSManager = new RSSManager(this);
    qDebug()<<connect(mRSSManager,SIGNAL(updateAvailable(QUrl,int)),this,SLOT(handleUpdateAvailable(QUrl,int)));
    mSourceList.append(QUrl("http://labs.qt.nokia.com/feed/"));
    mSourceList.append(QUrl("http://mobile.engadget.com/rss.xml"));
    mSourceList.append(QUrl("http://feeds.feedburner.com/Symbian/Blog"));
    mSourceList.append(QUrl("http://rss.allaboutsymbian.com/news/rss2all.xml"));
}

void test_rssmanager::testAll()
{
    // add subscription
    for(int i=0;i<mSourceList.count();i++)
    {
         mRSSManager->addSubscription(FeedSubscription(mSourceList[i],1));
         mRSSManager->start(mSourceList[i]);
    }
    listAllSubscriptions();
    mRSSManager->startAll();
}

void test_rssmanager::handleUpdateAvailable(QUrl sourceUrl, int updateditems)
{
    qDebug()<<__FUNCTION__;
    qDebug()<<"updated items:"<<updateditems;
    qDebug()<<sourceUrl;
    RSSParser* parser = mRSSManager->parser(sourceUrl);
    if(parser->isValid())
    {
qDebug()<<"\n--------------------------------------------------------------------";
        qDebug()<<parser->channelElement(RSSParser::title);
        qDebug()<<parser->itemElement(1,RSSParser::title).toUtf8();
        qDebug()<<parser->itemElement(1,RSSParser::pubDate).toUtf8();
qDebug()<<"--------------------------------------------------------------------\n";
    }
    parser->deleteLater();
}

// eof
