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
    ~RSSManager();

public slots:
    bool add(QUrl url,int msec = 60*1000);
    bool remove(QUrl url);
    void removeAll();

    bool start(QUrl url);
    void startAll();

    void stop(QUrl url);
    void stopAll();

    int count() const;
    bool contains(QUrl url) const;
    QList<QUrl> urls() const;
    QString feedFileName(QUrl url);
    RSSParser* parser(QUrl url);

    void setInterval(QUrl url, int msec);
    void updateAll();
    void update(QUrl url);

signals:
    /*!
      \brief Signal is emitted when new feed content is available
      \arg url Subscription URL on which new feed content is available.
      \arg updateditems Number of feed items that are updated.
      */
    void updateAvailable(QUrl url, int updateditems);
    void error(QString errorDescription,QUrl url);

private slots:
    // Internal methods, experimental support
    bool externalize();
    bool internalize();

private:
    RSSManagerPrivate* d;
};
#endif // RSSMANAGER_H
