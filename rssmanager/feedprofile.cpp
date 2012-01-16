#include <QTimer>
#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QBuffer>
#include <QFile>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#ifdef Q_OS_SYMBIAN
#include <QNetworkProxyFactory>
#endif
#include "rssmanager.h"
#include "feedprofile.h"
#include "rssparser.h"

QMutex mutex;

FeedProfile::FeedProfile(QUrl url,int interval,QObject *parent) :
    QObject(parent),
    mSourceUrl(url),
    mInterval(interval),
    mNetworkManager(NULL),
    mNetworkReply(NULL),
    mParser(NULL),
    mCacheInvalidated(true),
    mCachedCount(-1),
    mFeedReachable(false),
    mSmartUpdate(true)
{
#ifdef Q_OS_SYMBIAN
    QNetworkProxyFactory::setUseSystemConfiguration(true);
#endif
    setNetworkRequestActive(false);
    mParser = new RSSParser(this);
    mNetManCreatedCount = 0; // test only
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(handleTimeOut()));
}

FeedProfile::~FeedProfile() {
    mTimer.stop();
    if(isNetworkRequestActive())  {
        mNetworkManager->deleteLater();
        mNetworkReply->deleteLater();
    }
}

bool FeedProfile::isValid() const {
    // Feed with negative interval is valid
    return mSourceUrl.isValid() && mFeedReachable;
}

RSSParser* FeedProfile::parser() const {
    if(!mParser->source()) {
        // Each profile acts on a singel xmlsource,
        // so setting it once for ever.
        // TODO: test this with quick update intervals.
        QFile* src = new QFile(feedFileName(),mParser);
        if(src->open(QIODevice::ReadOnly))
            mParser->setSource(src);
    }
    // TODO: should we reset read head to beginning of the file?
    return mParser;
}

/*!
  Updates the feed irresptive of interval. If start() is called prior to this method it will not effect the interval.
  Hence a feed with negative interval is also updated.
  Use this method for an on demand update.
  **/
void FeedProfile::update() {
    // Ignore if a request is already active
    if(!isNetworkRequestActive())     {
        handleTimeOut();
    }
}

void FeedProfile::start() {
    updateTimer(mInterval);
    if(mInterval >= 0)
        update();
}

void FeedProfile::stop() {
    updateTimer(-1);
}

void FeedProfile::updateTimer(int msec) {
    mTimer.stop();
    if(isNetworkRequestActive() && mNetworkReply)
        mNetworkReply->abort();
    // start timer if it is a valid interval
    if(msec>=0) {
        mTimer.setInterval(msec);
        mTimer.start();
    }
    // negative timer value tells to stop updates
}

bool FeedProfile::isActive() const {
    return mTimer.isActive();
}

void FeedProfile::handleTimeOut() {
    // ignore
    if(isNetworkRequestActive())
        return;

    mCacheInvalidated = true;
    // Fetch feed from source
    mNetworkManager = new QNetworkAccessManager(this); mNetManCreatedCount++;
    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)),Qt::UniqueConnection);
    connect(mNetworkManager,SIGNAL(destroyed(QObject*)),
            this,SLOT(handleNetworkMgrDestroyed(QObject*)),Qt::UniqueConnection);
    mNetworkReply = mNetworkManager->get(QNetworkRequest(mSourceUrl));
    setNetworkRequestActive(true);
}

void FeedProfile::replyFinished(QNetworkReply *reply) {
    QMutex m;
    m.lock();
        setNetworkRequestActive(false);
            // No error
            if(QNetworkReply::NoError == reply->error()) {
                // read contents
                QByteArray content = reply->readAll();
                handleContent(content);
            } else {
                // handle error
                qWarning()<<__PRETTY_FUNCTION__<<":"<<reply->errorString();
                emit error(reply->errorString(),mSourceUrl.toString());
            }
        reply->deleteLater();
        // Feeds are usually gathered in periodic intervals.
        // So network manager need not reside in memory till it is required.
        mNetworkManager->deleteLater();
    m.unlock();
}

void FeedProfile::handleContent(QByteArray content) {
    if(content.size()) {
            QFile feedFile(feedFileName());
            if(feedFile.exists())
                feedFile.remove();

            if(feedFile.open(QIODevice::ReadWrite)) {
                QString tmp(content);
                feedFile.write(tmp.toUtf8());
                feedFile.close();
            } else {
                emit error("Cannot store the feed",mSourceUrl.toString());
                return;
            }

            int newItemsCount=0;
            QFile readFeedFile(feedFileName());
            readFeedFile.open(QIODevice::ReadOnly);
            RSSParser* parser = new RSSParser(this);
            parser->setSource(&readFeedFile);
            QStringList titles = parser->itemElements(RSSParser::title);
            bool perr = parser->isError();
            parser->deleteLater();
            readFeedFile.close();

            if(perr) {
                emit error("Unable to parse",mSourceUrl.toString());
                return;
            }

            int totalItems = titles.count();
            if(totalItems) {
                mFeedReachable = true;
                // Assume all items are new
                newItemsCount = totalItems;

                // Check for updates
                if(!mLatestElementTitle.isEmpty() && (titles.indexOf(mLatestElementTitle) >= 0))
                    newItemsCount = titles.indexOf(mLatestElementTitle);
                // New updates available
                if(newItemsCount) {
                    // XQuery numbering starts with 1, so we can send count as such
                    mLatestElementTitle = titles[0];
                    // emit this signal even if there is no active timer.
                    // some clients may need update on demand.
                    emit updateAvailable(mSourceUrl,newItemsCount);
                } else if(mSmartUpdate && 0 == newItemsCount) {
                    emit updateAvailable(mSourceUrl,totalItems);
                }

            } else {
                emit error(tr("Cannot parse feed"),mSourceUrl.toString());
            }
     }
}

QString FeedProfile::feedFileName() const {
    QString filename;
    filename.setNum( qHash(mSourceUrl.toString()) );
    filename.append(".xml");
    return filename;
}

int FeedProfile::count() const {
    // TODO: do caching and return result
    if(mCacheInvalidated || -1 == mCachedCount) {
        mCachedCount = parser()->count();
        mCacheInvalidated = false;
    }
    return mCachedCount;
}

QUrl FeedProfile::url() const {
    return mSourceUrl;
}

int FeedProfile::interval() const {
    return mInterval;
}

void FeedProfile::setSmartUpdate(bool val) {
    mSmartUpdate = val;
}

bool FeedProfile::hasSmartUpdate() const {
    return mSmartUpdate;
}

// test slot
void FeedProfile::handleNetworkMgrDestroyed(QObject *obj) {
    QMutex m;
    m.lock();
    mNetManCreatedCount--;
    if(obj)
        obj->objectName();
    m.unlock();
}

void FeedProfile::setNetworkRequestActive(bool value) {
    QMutex m;
    m.lock();
    mNetworkRequestActive = value;
    m.unlock();
}

bool FeedProfile::isNetworkRequestActive() {
    return mNetworkRequestActive;
}


// eof
