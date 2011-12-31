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
    explicit FeedProfile(FeedSubscription subscription,QObject *parent = 0);
    ~FeedProfile();
    bool isValid()
    {
        // Feed with negative interval is valid
        return !mSubscription.sourceUrl().toString().isEmpty();
    }
    RSSParser* parser();
    void update();
    QString feedFileName();

signals:
    void updateAvailable(QUrl sourceUrl, int updatedItems);
    void error(QString errorDescription,QUrl sourceUrl);

public slots:
    FeedSubscription subscription(){return mSubscription;}

    /** \param value less than zero stops the timer and all updates
     **/
    void updateTimer(int mins);
    bool isActive(){return mTimer.isActive();}
    QString lastestItemTitle(){return mLatestElementTitle;}


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
    FeedSubscription mSubscription;
    QString mLatestElementTitle;
    QString mFeedFileName;
    QTimer mTimer;
    QNetworkAccessManager* mNetworkManager;
    QNetworkReply* mNetworkReply;
    // Dont use this directly, use setter and getter instead
    bool mNetworkRequestActive;
    // used only for test
    int mNetManCreatedCount;
};

#endif // FEEDPROFILE_H
