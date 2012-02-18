// author: srikanthsombhatla@gmail.com
#ifndef RSSMANAGER_H
#define RSSMANAGER_H
#include <QObject>
#include <QUrl>
#include <QHash>
#include "rssparser.h"


// Forward declaration
class FeedProfile;
class RSSManagerPrivate;
class RSSManager : public QObject
{
    Q_OBJECT
public:
    explicit RSSManager(QObject *parent = 0);
    static RSSManager* instance();
    static QString storagePath();
    ~RSSManager();

public slots:
    bool add(QUrl url,int msec = 3600000);
    bool remove(QUrl url);
    void removeAll();

    bool start(QUrl url);
    void startAll();

    void stop(QUrl url);
    void stopAll();

    int count() const;
    bool contains(QUrl url) const;
    QList<QUrl> urls() const;
    QString feedFileName(QUrl url) const;
    RSSParser* parser(QUrl url);

    void setInterval(QUrl url, int msec);
    void updateAll();
    void update(QUrl url);
    void setUpdateOnNewItemsOnly(QUrl url, bool val);
    bool isUpdatingOnNewItemsOnly(QUrl url) const;

    bool saveState();
    bool restoreState();
    bool clearState();
    void fetchUpdates();

signals:
    /*!
      \brief Signal is emitted when new feed content is available
      \arg url Subscription URL on which new feed content is available.
      \arg newItemsCount Number of feed items that are newly added after update.
      */
    void updateAvailable(QUrl url, int newItemsCount);
    void error(QString errorDescription,QUrl url);

private:
    RSSManagerPrivate* d;
};
#endif // RSSMANAGER_H
