// author: srikanthsombhatla@gmail.com
#ifndef RSSMANAGER_H
#define RSSMANAGER_H
#include <QObject>
#include <QUrl>
#include <QHash>
#include "rssparser.h"


/**
  Class to hold a RSS Subscrition info
  **/
class FeedSubscription
{
public:
    explicit FeedSubscription(QUrl sourceUrl,int updateIntervalInMins = 60)
    {
        mSourceUrl = sourceUrl;
        mUpdateInterval = updateIntervalInMins;
    }

    QUrl sourceUrl(){return mSourceUrl;}
    void setSourceUrl(QUrl source){mSourceUrl = source;}
    int updateInterval(){return mUpdateInterval;}
    void setUpdateInterval(int updateIntervalInMins){mUpdateInterval = updateIntervalInMins;}

    bool isValid()
    {
        // Feed with negative interval is valid
        return !mSourceUrl.toString().isEmpty();
    }
private:
    QUrl mSourceUrl;
    int mUpdateInterval;
};

// Forward declaration
class FeedProfile;
class RSSManager : public QObject
{
    Q_OBJECT
public:
    explicit RSSManager(QObject *parent = 0);
    ~RSSManager();
    void addFeed(FeedSubscription newSubscription);
    bool setUpdateInterval(QUrl sourceUrl, int newUpdateIntervalInMins);
    RSSParser* parser(QUrl sourceUrl);
    bool isFeedValid(QUrl sourceUrl);
    bool start(QUrl sourceUrl);
    void startAll();
    bool stop(QUrl sourceUrl);
    void stopAll();
    bool removeFeed(QUrl sourceUrl);
    FeedSubscription feed(QUrl sourceUrl);
    QList<FeedSubscription> feeds();
    QList<QUrl> feedUrls();
    QString feedFileName(QUrl sourceUrl);
    int feedsCount(){return mFeedProfiles.count();}
    void updateAll();
    void update(QUrl sourceUrl);

signals:
    /*!
      \brief Signal is emitted when new feed content is available
      \arg sourceUrl Subscription URL on which new feed content is available.
      \arg updateditems Number of feed items that are updated.
      */
    void updateAvailable(QUrl sourceUrl, int updateditems);
    void error(QString errorDescription,QUrl sourceUrl);

private slots:
    // Internal methods, experimental support
    bool externalize();
    bool internalize();

private:
    QHash<QString,FeedProfile*> mFeedProfiles;
};
#endif // RSSMANAGER_H
