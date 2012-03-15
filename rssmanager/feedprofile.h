#ifndef FEEDPROFILE_H
#define FEEDPROFILE_H
#include <QObject>
#include <QTimer>
#include "rssmanager.h"

class QNetworkAccessManager;
class QNetworkReply;
class RSSParser;

/**
  Internal class for handling feeds
  */
class FeedProfile : public QObject
{
    Q_OBJECT
public:
    explicit FeedProfile(QUrl url,int interval,RSSManager* mgr,QObject *parent = 0);
    ~FeedProfile();
    bool isValid() const;
    void update();
    QString feedFileName() const;
    //int count() const;
    QUrl url() const;
    int interval() const;
    void setSmartUpdate(bool val);
    bool hasSmartUpdate() const;

signals:
    void updateAvailable(QUrl sourceUrl, int updatedItems);
    void error(QString errorDescription,QUrl sourceUrl);

public slots:
    void start();
    void stop();
    void updateTimer(int msec);
    /** \param value less than zero stops the timer and all updates
     **/
    bool isActive() const;
    QString latestItemCheck() const;
    void setLatestItemCheck(QString check);
    void setUserData(FeedUserData userData);
    FeedUserData userData() const;

private slots:
    void handleTimeOut();
    void replyFinished(QNetworkReply* reply);
    void handleContent(QByteArray content);

    // test slots
    void handleNetworkMgrDestroyed(QObject* obj);
private:
    void setNetworkRequestActive(bool value);
    bool isNetworkRequestActive();

private:
    const QUrl mSourceUrl;
    int mInterval;
    RSSManager* mRSSManager;// non-owning
    QString mLatestElementCheck;
    QString mFeedFileName;
    QTimer mTimer;
    QNetworkAccessManager* mNetworkManager;
    QNetworkReply* mNetworkReply;
    FeedUserData mUserData;
    mutable int mCachedCount;
    mutable bool mCacheInvalidated;
    // Dont use this directly, use setter and getter instead
    bool mNetworkRequestActive;
    // used only for test
    int mNetManCreatedCount;
    int mFeedReachable;
    bool mSmartUpdate;
};

#endif // FEEDPROFILE_H
