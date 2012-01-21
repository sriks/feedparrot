#include <QTimer>
#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QBuffer>
#include <QFile>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QNetworkProxyFactory>
#include "rssmanager.h"
#include "feedprofile.h"
#include "rssparser.h"

//#define START_FETCH_WHEN_CREATED;

const int KOneMinInMSec = 60000;
QMutex mutex;

FeedProfile::FeedProfile(FeedSubscription subscription,QObject *parent) :
    QObject(parent),
    mSubscription(subscription),
    mNetworkManager(NULL),
    mNetworkReply(NULL)
{
#ifdef Q_OS_SYMBIAN
    QNetworkProxyFactory::setUseSystemConfiguration(true);
#endif
    setNetworkRequestActive(false);
    mNetManCreatedCount = 0; // test only
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(handleTimeOut()));
#ifdef START_FETCH_WHEN_CREATED
    // start initial fetch
    if(mSubscription.updateInterval())
    { update(); }
#endif
}

FeedProfile::~FeedProfile()
{
    qDebug()<<__PRETTY_FUNCTION__;
    mTimer.stop();
    if(isNetworkRequestActive())
    {
    mNetworkManager->deleteLater();
    mNetworkReply->deleteLater();
    qDebug()<<"Netman creation mark "<<mNetManCreatedCount;
    }
}

RSSParser* FeedProfile::parser()
{
    qDebug()<<__FUNCTION__;
    RSSParser* parser = new RSSParser;
    QFile* feedFile = new QFile(feedFileName(),parser);
    if(feedFile->open(QIODevice::ReadOnly))
    {
        parser->setSource(feedFile);
    }
    return parser;
}

void FeedProfile::update()
{
    // Ignore if a request is already active
    if(!isNetworkRequestActive())
    {
        // This is an on-demand update, so dont restart the timer.
        // We have to update the feed even if update interval is negative.
        // A client may just need to update on demand and not in periodic interval.
        handleTimeOut();
    }
}

void FeedProfile::updateTimer(int mins)
{
    mTimer.stop();

    // start timer if it is a valid interval
    if(mins>=0)
    {
    mTimer.setInterval(mins * KOneMinInMSec);
    mTimer.start();
    }

    // negative timer value tells to stop updates
    else
    {
        if(isNetworkRequestActive())
        {mNetworkReply->abort();}
    }
}

void FeedProfile::handleTimeOut()
{
    qDebug()<<__FUNCTION__;
    qDebug()<<mSubscription.sourceUrl();
    // ignore
    if(isNetworkRequestActive()) { return; }

    // Fetch feed from source
    mNetworkManager = new QNetworkAccessManager(this); mNetManCreatedCount++;
    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    connect(mNetworkManager,SIGNAL(destroyed(QObject*)),this,SLOT(handleNetworkMgrDestroyed(QObject*)));
    mNetworkReply = mNetworkManager->get(QNetworkRequest(mSubscription.sourceUrl()));
    setNetworkRequestActive(true);
}

void FeedProfile::replyFinished(QNetworkReply *reply)
{
    qDebug()<<__FUNCTION__;
    QMutex m;
    m.lock();
        setNetworkRequestActive(false);
        qDebug()<<reply->request().url();
            // No error
            if(QNetworkReply::NoError == reply->error())
            {
                // read contents
                QByteArray content = reply->readAll();
                handleContent(content);
            }

            // handle error
            else
            {
                qWarning()<<reply->errorString();
                emit error(reply->errorString(),mSubscription.sourceUrl());
            }
        reply->deleteLater();
        // Feeds are usually gathered in periodic intervals.
        // So network manager need not reside in memory till it is required.
        mNetworkManager->deleteLater();
    m.unlock();
}

void FeedProfile::handleContent(QByteArray content)
{
  qDebug()<<__FUNCTION__;
    // valid content
    if(content.size())
    {
            QFile feedFile(feedFileName());
            if(feedFile.exists())
            {
                feedFile.remove();
            }

            if(feedFile.open(QIODevice::ReadWrite))
            {
                QString tmp(content);
                feedFile.write(tmp.toUtf8());
                feedFile.close();
            }

            else
            {
                emit error("Cannot store the feed",mSubscription.sourceUrl());
                return;
            }

            int newItemsCount=0;
            QFile readFeedFile(feedFileName());
            qDebug()<<readFeedFile.open(QIODevice::ReadOnly);
            RSSParser* parser = new RSSParser(this);
            parser->setSource(&readFeedFile);
            QStringList titles = parser->itemElements(RSSParser::title);
            parser->deleteLater();
            readFeedFile.close();

            int totalItems = titles.count();
            if(totalItems)
            {
                // Assume all items are new
                newItemsCount = totalItems;

                // Check for updates
                if(!mLatestElementTitle.isEmpty() && (titles.indexOf(mLatestElementTitle) >= 0))
                {
                    newItemsCount = titles.indexOf(mLatestElementTitle);
                }

                // New updates available
                if(newItemsCount)
                {
                    // XQuery numbering starts with 1, so we can send count as such
                    mLatestElementTitle = titles[0];
                    // emit this signal even if there is no active timer.
                    // some clients may need update on demand.
                    emit updateAvailable(mSubscription.sourceUrl(),newItemsCount);
                }
            }

            // Parsing error, No items found in parsing
            else
            {
                emit error(tr("Cannot parse feed"),mSubscription.sourceUrl());
            }

     }
}

QString FeedProfile::feedFileName()
{
    QString filename;
    filename.setNum( qHash(mSubscription.sourceUrl().toString()) );
    filename.append(".xml");
    return filename;
}

// test slot
void FeedProfile::handleNetworkMgrDestroyed(QObject *obj)
{
    qDebug()<<__FUNCTION__;
    QMutex m;
    m.lock();
    mNetManCreatedCount--;
    qDebug()<<"deleted count "<<mNetManCreatedCount;

        if(obj)
        {
            obj->objectName();
        }
    m.unlock();
}

void FeedProfile::setNetworkRequestActive(bool value)
{
    QMutex m;
    m.lock();
    mNetworkRequestActive = value;
    m.unlock();
}

bool FeedProfile::isNetworkRequestActive()
{
    return mNetworkRequestActive;
}

// eof
