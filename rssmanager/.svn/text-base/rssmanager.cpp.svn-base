#include <QDebug>
#include <QMetaType>
#include <QXmlStreamWriter>
#include <QFile>
#include "rssmanager.h"
#include "feedprofile.h"

// Internal class to externalize and internalize feed subscription preferences
class FeedStorage
{
public:
    FeedSubscription mFeedSubscription;
    QString mLatestItemTitle;
};
const QString KFeedStorageFileName("feedstore.xml");
const QString KFeedStorageFeedElement("feed");
const QString KFeedStorageSourceUrl("sourceurl");
const QString KFeedStorageUpdatePeriod("updateperiod");
const QString KFeedStorageLatestItemTitle("latestitemtitle");

const QString KOrganisationName("decii");
const QString KAppName("rssmanager");
const QString KFeedGroup("feeds");
const QString KFeedList("feedlist");

RSSManager::RSSManager(QObject *parent) :
    QObject(parent)
{
    // read all preferences
    //internalize();
}

RSSManager::~RSSManager()
{
    // write all preferences
    qDebug()<<__FUNCTION__;
    externalize();
}

bool RSSManager::externalize()
{
    // Write all subscriptions to persistent storage as XML
    QFile storageFile(KFeedStorageFileName, this);
    if(storageFile.open(QIODevice::WriteOnly))
    {
    QXmlStreamWriter xmlWriter(&storageFile);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    QList<FeedProfile*>  profiles = mFeedProfiles.values();
        for(int i=0;i<profiles.count();i++)
        {
            xmlWriter.writeStartElement(KFeedStorageFeedElement);
            xmlWriter.writeTextElement(KFeedStorageSourceUrl,
                                       profiles.at(i)->subscription().sourceUrl().toString());
            xmlWriter.writeTextElement(KFeedStorageUpdatePeriod,
                                       QString().setNum(profiles.at(i)->subscription().updateInterval()));
            xmlWriter.writeTextElement(KFeedStorageLatestItemTitle,
                                       profiles.at(i)->lastestItemTitle());
            xmlWriter.writeEndElement();
        }
    xmlWriter.writeEndDocument();
    storageFile.close();
    return true;
    }

    // file not opened
    else
    {
        return false;
    }
}

bool RSSManager::internalize()
{
    return true;
}

/*!
\brief Adds a feed. If the feed already exist, it replaces existing feed.
Note: This will not initiate update cycle. Client should call start() to initiate updates.
\sa start() \sa startAll()
*/
void RSSManager::addFeed(FeedSubscription newSubscription)
{
    FeedProfile* profile = new FeedProfile(newSubscription,this);
    connect(profile,SIGNAL(updateAvailable(QUrl,int)),this,SIGNAL(updateAvailable(QUrl,int)));
    connect(profile,SIGNAL(error(QString,QUrl)),this,SIGNAL(error(QString,QUrl)));
    mFeedProfiles.insert(newSubscription.sourceUrl().toString(),profile);
}

/*!
\brief Sets new update interval to the specified feed.
       This will not initiate a fetch. Client should call update() to initiate a fetch on demand.
\arg sourceUrl URL on which subscription interval should be changed.
\arg newUpdateIntervalInMins new update interval in minutes.
     If a negative value is supplied, subscription is stopped.
\return returns false if supplied sourceUrl is invalid or not added yet.
\sa update()
*/
bool RSSManager::setUpdateInterval(QUrl sourceUrl, int newUpdateIntervalInMins)
{
    qDebug()<<__FUNCTION__;
    if(isFeedValid(sourceUrl))
    {
        mFeedProfiles[sourceUrl.toString()]->updateTimer(newUpdateIntervalInMins);
        return true;
    }
    qWarning()<<"Invalid feed "<<sourceUrl;
return false;
}

/*!
\brief Removes feed permanently. Stops update cycle of this feed
       If feed is fetched and being processed, updateAvailable signal is emitted.
\arg sourceUrl Feed to be removed
\return true if sucessfully removed
        false if sourceUrl is not yet added to RSSManager.
*/
bool RSSManager::removeFeed(QUrl sourceUrl)
{
    if(isFeedValid(sourceUrl))
    {
    mFeedProfiles[sourceUrl.toString()]->deleteLater();
    mFeedProfiles.remove(sourceUrl.toString());
    return true;
    }
// return that no such feed exists
return false;
}

/*!
\brief Returns the feed identified by \p sourceUrl
       If no url mentioned in \p sourceUrl exists, it returns an invalid FeedSubscription.
       Hence client should check as FeedSubscription::isValid() before using it.
\arg sourceUrl Feed Url identifying the feed.
\return valid FeedSubscription if \p sourceUrl is valid
\sa FeedSubscription::isValid()
*/
FeedSubscription RSSManager::feed(QUrl sourceUrl)
{
    FeedProfile defaultProfile(FeedSubscription(QUrl(),-1));
    FeedProfile* profile = mFeedProfiles.value(sourceUrl.toString(),&defaultProfile);
    return profile->subscription();
}

/*!
\brief Returns all feeds added so far as a QList<FeedSubscription>
\sa feed()
*/
QList<FeedSubscription> RSSManager::feeds()
{
    QList<FeedSubscription> subscriptionList;
    QList<FeedProfile*>  profiles = mFeedProfiles.values();
    for(int i=0;i<profiles.count();i++)
    {
        subscriptionList.append(profiles.at(i)->subscription());
    }

    return subscriptionList;
}

/*!
\brief Returns all URLs of all feeds added so far.
\sa feed()
\sa feeds()
*/
QList<QUrl> RSSManager::feedUrls()
{
    QList<QUrl> urlList;
    QList<FeedProfile*>  profiles = mFeedProfiles.values();
    for(int i=0;i<profiles.count();i++)
    {
    urlList.append(profiles[i]->subscription().sourceUrl());
    }
return urlList;
}

QString RSSManager::feedFileName(QUrl sourceUrl)
{
    FeedProfile defaultProfile(FeedSubscription(QUrl(),-1));
    FeedProfile* profile = mFeedProfiles.value(sourceUrl.toString(),&defaultProfile);
    return profile->feedFileName();
}

/*!
\brief Updates all feeds which are added so far.
       Order of update is undefined. If such an order is required prefer to use update()
\sa update()
*/
void RSSManager::updateAll()
{
    qDebug()<<__FUNCTION__;
    QList<QString> keys = mFeedProfiles.uniqueKeys();
    for(int i=0;i<keys.count();i++)
    {
        update(QUrl(keys[i]));
    }
}

void RSSManager::update(QUrl sourceUrl)
{
    qDebug()<<__FUNCTION__;
    qDebug()<<sourceUrl;
    QString key = sourceUrl.toString();
    if(mFeedProfiles.contains(key))
    {
        mFeedProfiles[key]->update();
    }
}

/*!
\brief Provides a parser initialized and ready to parse results from sourceurl.
\attention Ownership is transfered to the client. Client should call deleteLater() when finished with parsing.
\arg sourceUrl URL for which the results should be parsed.
\return returns initialized ready to use RSSParser. Returns 0 for inivaid sourceUrl.
*/
RSSParser* RSSManager::parser(QUrl sourceUrl)
{
    FeedProfile defaultProfile(FeedSubscription(QUrl(),-1));
    RSSParser* parser =  mFeedProfiles.value(sourceUrl.toString(),&defaultProfile)->parser();
    return parser;
}
/*!
\brief Stop update.
       Calling start() will start updates again.
\arg sourceUrl URL on which update cycle should be stopped.
\return true if a valid sourceUrl.
\attention This has no effect if a feed is already fetched and being processed.
           Hence a return value of true should not be considered as "fetching stopped".
\sa stopAll() start() startAll() update() updateAll()
*/
bool RSSManager::stop(QUrl sourceUrl)
{
    return setUpdateInterval(sourceUrl,-1);
}

/*!
\brief Convinience method to stop update cycle on all feeds
\arg sourceUrl URL on which update cycle should be stopped.
\attention This has no effect if a feed is already fetched and being processed.
\sa stop()
*/
void RSSManager::stopAll()
{
    QList<QString> keys = mFeedProfiles.uniqueKeys();
    for(int i=0;i<keys.count();i++)
    {
        stop(QUrl(keys[i]));
    }
}

/*!
\brief Starts update. Clients should call this method to start update cycle.
       Once called update cycle is started and feeds are fetched as per update interval.
       This has no effect if called on a feed which is already started.
\arg sourceUrl URL on which update cycle should start.
\return true, if successfully started or called on a feed which is started.
        false, if an invalid sourceUrl is supplied.
\sa startAll()
*/
bool RSSManager::start(QUrl sourceUrl)
{
    // TODO: Feed validation is done two times. Optimize it.
    qDebug()<<__FUNCTION__;
    qDebug()<<sourceUrl.toString();
    if(isFeedValid(sourceUrl))
    {
    // ignore if it is already active
    if(!mFeedProfiles[sourceUrl.toString()]->isActive())
        {
        bool stat = setUpdateInterval(sourceUrl,
                                   mFeedProfiles[sourceUrl.toString()]->subscription().updateInterval());
        if(stat)
        {
        update(sourceUrl.toString());
        }
        return stat;
        }
    qWarning()<<"Starting an active feed "<< sourceUrl;
    return true;
    }

    return false;
}

/*!
\brief Convience method to start all feeds
\sa start()
*/
void RSSManager::startAll()
{
    QList<QString> keys = mFeedProfiles.uniqueKeys();
    for(int i=0;i<keys.count();i++)
    {
        start(QUrl(keys[i]));
    }
}

/*!
\brief returns true if a feed is valid.
       A feed is invalid if the supplied sourceUrl is not added to RSSManager

\sa addSubscription();
*/
bool RSSManager::isFeedValid(QUrl sourceUrl)
{
    if(mFeedProfiles.contains(sourceUrl.toString()))
    {
        return mFeedProfiles[sourceUrl.toString()]->isValid();
    }

    qWarning()<<__FUNCTION__<<" invalid feed "<<sourceUrl;
    return false;
}

// end of file
