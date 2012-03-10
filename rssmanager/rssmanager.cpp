#include <QDebug>
#include <QMetaType>
#include <QXmlStreamWriter>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include "rssmanager.h"
#include "feedprofile.h"

#if defined(DC_HARMATTAN)
QString DATA_LOCATION = "/home/user/.local/share//data/";
#endif

class RSSManagerPrivate {
public:
    QString appName;
    QMap<QUrl,FeedProfile*> profiles;
    FeedProfile* profile(QUrl url) const {
        return profiles.value(url,NULL);
    }
    ~RSSManagerPrivate() {
        qDeleteAll(profiles.begin(),profiles.end());
    }
};

const QString KFeedStorageFileName("feedstore.xml");
const QString KFeedStorageFeedElement("feedstore");
const QString KFeedItem("item");
const QString KFeedStorageurl("link");
const QString KFeedStorageUpdatePeriod("updateperiod");
const QString KFeedStorageLatestItemTitle("title");
const QString KFeedStorageUserDataItemTitle("userdata");
const QString KFeedStorageCountItemTitle("count");
const QString KFeedStorageKeyItemTitle("key");
const QString KFeedStorageValueItemTitle("value");

const QString KOrganisationName("decii");
const QString KAppName("rssmanager");
const QString KFeedGroup("feeds");
const QString KFeedList("feedlist");

RSSManager::RSSManager(QString applicationName,QObject *parent) :
    QObject(parent) {
    d = new RSSManagerPrivate;
    d->appName = applicationName;
    restoreState();
}

/*!
  Creates an instance of RSSManager.
  Onwership is transferred to the caller.
  If RSSManager is created using new, calling instance() will return a new instance rather than the instance created with new.
  Hence use instance() and new() in a mutually exclusive manner.
  **/
RSSManager *RSSManager::instance(QString applicationName) {
    static RSSManager* m = NULL;
    if(!m)
        m = new RSSManager(applicationName);
    return m;
}

/*!
  Returns storage path where feed related data can be stored.
  Clients need to remove this path when uninstalling in platforms like Harmattan.

     basePath is <storagepath>/<appname>/tmp
     Storage path for different platforms...
        1.Symbian is <privatefolder>
        2.Harmattan is /home/user/.local/share//data/<appname>
        3.Others same as QCoreApplication::applicationDirPath()

        For example in harmattan if the application name is "TechNews" then storagePath would return
        /home/user/.local/share//data/TechNews
  **/
QString RSSManager::storagePath() {
    static QString basePath;
    if(basePath.isEmpty()) {
        #if defined(DC_HARMATTAN)
        basePath = DATA_LOCATION;
        #else
        basePath = QCoreApplication::applicationDirPath();
        #endif

        QString appname = applicationName();
        if(appname.isEmpty()) {
            qWarning()<<"Required to speicfy appname using RSSManager::setApplicationName().";
            basePath.clear();
            return QString();
        }

        QDir d(basePath);
        if(!d.cd(appname)) {
            if(!d.mkdir(appname)) {
                qWarning()<<Q_FUNC_INFO<<"Unable to create folder "<<appname<<" in "<<basePath;
                basePath.clear();
                return QString();
            } else {
                d.cd(appname);
            }
        }
        basePath = d.absolutePath();
    }
    return basePath;
}

RSSManager::~RSSManager() {
    // write all preferences
    qDebug()<<Q_FUNC_INFO;
    saveState();
    delete d;
}

/*!
  Saves state to local storage
  **/
bool RSSManager::saveState() {
    // Write all subscriptions to persistent storage as XML
    QString basePath = storagePath();
    if(basePath.isEmpty())
        return false;

    QFile storageFile(basePath + "/" + KFeedStorageFileName);
    if(storageFile.open(QIODevice::WriteOnly)) {
        QXmlStreamWriter xmlWriter(&storageFile);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement(KFeedStorageFeedElement);
        QList<FeedProfile*>  profiles = d->profiles.values();
            for(int i=0;i<profiles.count();i++) {
                FeedProfile* profile = profiles.at(i);
                xmlWriter.writeStartElement(KFeedItem);
                xmlWriter.writeTextElement(KFeedStorageurl,
                                           profiles.at(i)->url().toString());
                xmlWriter.writeTextElement(KFeedStorageUpdatePeriod,
                                           QString().setNum(profiles.at(i)->interval()));
                xmlWriter.writeTextElement(KFeedStorageLatestItemTitle,
                                           profiles.at(i)->latestItemTitle());

                // user data
                if(!profile->userData().isEmpty()) {
                    FeedUserData userdata(profile->userData());
                    QMapIterator<QString,QString> iter(userdata);
                    xmlWriter.writeStartElement(KFeedStorageUserDataItemTitle);
                    xmlWriter.writeTextElement(KFeedStorageCountItemTitle,
                                               QString().setNum(userdata.size()));
                    while(iter.hasNext()) {
                        iter.next();
                        xmlWriter.writeTextElement(KFeedStorageKeyItemTitle,
                                                   iter.key());
                        xmlWriter.writeTextElement(KFeedStorageValueItemTitle,
                                                   iter.value());
                    }
                    xmlWriter.writeEndElement();
                }
                xmlWriter.writeEndElement();
            }
        xmlWriter.writeEndDocument();
        storageFile.close();
        return true;
    }

    // file not opened
    else {
        return false;
    }
}

/*!
  Reloads from saved state
  **/
bool RSSManager::restoreState() {
    QString basePath = storagePath();
    if(basePath.isEmpty())
        return false;

    QFile storageFile(basePath + "/" + KFeedStorageFileName);
    if(storageFile.open(QIODevice::ReadOnly)) {
        RSSParser* p = new RSSParser(this);
        p->setSource(&storageFile);
        int c = p->count();
        for(int i=0;i<c;++i) {
            QUrl url(p->itemElement(i,KFeedStorageurl).trimmed());
            QString title = p->itemElement(i,KFeedStorageLatestItemTitle).trimmed();
            QString invStr = p->itemElement(i,KFeedStorageUpdatePeriod).trimmed();
            add(url,invStr.toInt()); // TODO: check base when converting str to interval
            FeedProfile* fp = d->profile(url);
            if(fp) {
                fp->setLatestItemTitle(title);
                // setting user data
                int c = p->itemElement(i,KFeedStorageUserDataItemTitle+"/"+
                                         KFeedStorageCountItemTitle).trimmed().toInt();
                FeedUserData userdata;
                const QString keyValueIndexStr(KFeedStorageUserDataItemTitle+"/"+"%2"+"[%3]");
                for(int j=0;j<c;++j) {
                    QString queryForKey = keyValueIndexStr.arg(KFeedStorageKeyItemTitle,QString().setNum(j+1)); // xquery index starts with 1
                    QString key = p->itemElement(i,queryForKey).trimmed();
                    if(!key.isEmpty()) {
                        QString queryForvalue = keyValueIndexStr.arg(KFeedStorageValueItemTitle,QString().setNum(j+1));
                        QString value = p->itemElement(i,queryForvalue).trimmed();
                        userdata.insert(key,value);
                        fp->setUserData(userdata);
                    }
                }
            }
        }
        delete p;
    }
    return true;
}

/*!
  Clears saved state.
  **/
bool RSSManager::clearState() {
    QString basePath = storagePath();
    if(basePath.isEmpty())
        return false;
    return QFile::remove(basePath+"/"+KFeedStorageFileName);
}

/*!
  Fetches updates for feeds which were added in previous session.
  **/
void RSSManager::fetchUpdates() {
    restoreState();
    updateAll();
}

/*!
  Returns application name set during construction.
  **/
QString RSSManager::applicationName() const {
    return d->appName;
}

void RSSManager::setUserData(QUrl url, FeedUserData userData) {
    FeedProfile* p = d->profile(url);
    if(p && !userData.isEmpty())
        p->setUserData(userData);
}

FeedUserData RSSManager::userData(QUrl url) const {
    FeedUserData userdata;
    FeedProfile* p = d->profile(url);
    if(p)
        userdata = p->userData();
    return userdata;
}

QMap< QUrl, FeedUserData > RSSManager::userData() const {
    QMap< QUrl, FeedUserData > userdatamap;
    QMapIterator<QUrl,FeedProfile*> iter(d->profiles);
    while(iter.hasNext()) {
        iter.next();
        userdatamap.insert(iter.key(),userData(iter.key()));
    }
    return userdatamap;
}

/*!
  Adds new feed \a url to manager with interval \a msec. If does nothing if \a url is already added.
  \return returns true if added successfully.
  \note Since adding suplicate \a url is a NOOP, it will not restart the timer. To restart timer please
        use setInterval(QUrl,int)
  \sa remove(QUrl), removeAll()
  **/
bool RSSManager::add(QUrl url, int msec) {
    bool res = false;
    if(!contains(url) && !url.isEmpty()) {
        FeedProfile* profile = new FeedProfile(url,msec,this);
        connect(profile,SIGNAL(updateAvailable(QUrl,int)),this,SIGNAL(updateAvailable(QUrl,int)),Qt::UniqueConnection);
        connect(profile,SIGNAL(error(QString,QUrl)),this,SIGNAL(error(QString,QUrl)),Qt::UniqueConnection);
        d->profiles.insert(url,profile);
        res = true;
    }
    return res;
}

/*!
\brief Sets new update interval to \a msec to the feed identified by \a url.
       This restarts the feed with new interval \a msec.
       If a negative value is supplied, subscription is stopped.
       But prefer to use stop() which is more explicit.
\return returns false if supplied url is invalid or not added yet.
\sa update()
*/
void RSSManager::setInterval(QUrl url, int msec) {
    FeedProfile* f = d->profile(url);
    if(f)
        f->updateTimer(msec);
    else
        qWarning()<<Q_FUNC_INFO<<" url not added "<<url;
}

/*!
\brief Removes feed permanently. Stops update cycle of this feed
       If feed is fetched and being processed, updateAvailable signal is emitted.
\arg url Feed to be removed
\return true if sucessfully removed
        false if url is not yet added to RSSManager.
*/
bool RSSManager::remove(QUrl url)
{
    bool result = false;
    if(contains(url)) {
        d->profiles.value(url)->stop();
        delete d->profiles.take(url);
    }
return result;
}

void RSSManager::removeAll() {
    if(!d->profiles.isEmpty()) {
        QMapIterator<QUrl,FeedProfile*> iter(d->profiles);
        while(iter.hasNext())
            remove(iter.next().key());
    }
}

bool RSSManager::contains(QUrl url) const {
    return d->profiles.contains(url);
}

/*!
\brief Returns all URLs of all feeds added so far.
\sa feed()
\sa feeds()
*/
QList<QUrl> RSSManager::urls() const {
    return d->profiles.keys();
}

/*!
  Returns the filename in which this feed is stored on disk.
  File name will always have an extension .xml.
  It is advised not to do in-place modifications on the file, instead copy it.
  **/
QString RSSManager::feedFileName(QUrl url) const {
    QString name;
    if(d->profiles.contains(url))
        name = d->profiles.value(url)->feedFileName();
    return name;
}

int RSSManager::count() const {
    return d->profiles.size();
}

/*!
\brief Updates all feeds which are added so far.
       Order of update is undefined. If such an order is required prefer to use update(QUrl)
\sa update(QUrl)
*/
void RSSManager::updateAll() {
    QMapIterator<QUrl,FeedProfile*> iter(d->profiles);
    while(iter.hasNext())
        update(iter.next().key());
}

/*!
  Updates the feed on demand irresptive of interval. Interval is not restarted.
  Hence a feed with negative interval is also updated.
  Use this method for an on demand update.
  \sa updateAll(), start(QUrl), startAll()
  **/
void RSSManager::update(QUrl url) {
    FeedProfile* p = d->profile(url);
    if(p)
        p->update();
}

/*!
  Sets smart update to \a val.
  If this is set to true, updateAvailable() is emitted only if any new items are avaible.
  If set to false, updateAvailable() is emitted on every feed fetch irrespective of new items avaible in the fetch.
  The default value is true.
  \sa isUpdatingOnNewItemsOnly(QUrl)
  **/
void RSSManager::setUpdateOnNewItemsOnly(QUrl url, bool val) {
    FeedProfile* p = d->profile(url);
    if(p)
       p->setSmartUpdate(val);
}

/*!
  Returns whether \a url is updating on new items only.
  \sa setUpdateOnNewItemsOnly(QUrl,bool)
  **/
bool RSSManager::isUpdatingOnNewItemsOnly(QUrl url) const {
    bool res = false;
    FeedProfile* p = d->profile(url);
    if(p)
      res = p->hasSmartUpdate();
    return res;
}

/*!
\brief Provides a parser initialized and ready to parse results from url.
\attention Ownership is transfered to the client. Client should call deleteLater() when finished with parsing.
\arg url URL for which the results should be parsed.
\return returns initialized ready to use RSSParser. Returns 0 for inivaid url.
*/
RSSParser* RSSManager::parser(QUrl url) {
    RSSParser* parser = NULL;
    FeedProfile* p = d->profile(url);
    if(p) {
        parser = new RSSParser(this);
        QFile* src = new QFile(p->feedFileName());
        src->open(QIODevice::ReadOnly);
        parser->setSource(src);
    }
    return parser;
}

/*!
\brief Stops update for feed identified by \a url.
\returns true if successfull.
\note This has no effect if a feed is already fetched and being processed.
      Hence a return value of true should not be considered as "fetching stopped".
\sa stopAll() start() startAll() update() updateAll()
*/
void RSSManager::stop(QUrl url) {
    if(contains(url))
        d->profile(url)->stop();
}

/*!
\brief Stops all feed updates.
\note This has no effect if a feed is already fetched and being processed.
\sa stop(QUrl)
*/
void RSSManager::stopAll() {
    if(!d->profiles.isEmpty()) {
        QMapIterator<QUrl,FeedProfile*> iter(d->profiles);
        while(iter.hasNext()) {
            FeedProfile* f = iter.next().value();
            f->stop();
        }
    }
}

/*!
\brief Starts update. Clients should call this method to start update cycle.
       Once called update cycle is started and feeds are fetched as per update interval.
       This has no effect if called on a feed which is already started or the interval is negative.
\arg url URL on which update cycle should start.
\return true, if successfully started or called on a feed which is started.
        false, if an invalid url is supplied.
\sa startAll()
*/
bool RSSManager::start(QUrl url) {
    bool res = false;
    if(contains(url)) {
        d->profile(url)->start();
        res = true;
    }
    return res;
}

/*!
\brief Convience method to start all feeds.
\sa start(QUrl)
*/
void RSSManager::startAll() {
    if(!d->profiles.isEmpty()) {
        QMapIterator<QUrl,FeedProfile*> iter(d->profiles);
        while(iter.hasNext())
            start(iter.next().key());
    }
}

// end of file
