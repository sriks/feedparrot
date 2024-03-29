#ifndef RSSPARSER_H
#define RSSPARSER_H

#include <QObject>
#include <QXmlQuery>
#include <QStringList>

class QUrl;
class QIODevice;
class RSSParser : public QObject
{
    Q_OBJECT
    Q_ENUMS(RSSElement)

public:
    explicit RSSParser(QObject *parent = 0);
    ~RSSParser();
    /*! Enum describing different RSS elements.*/
    enum RSSElement
    {
        title,
        link,
        description,
        pubDate,
        language,
        lastBuildDate,
        generator,
        copyright,
        skipHours,
        skipDays,
        // Pure Item elements
        author,
        creator, // TODO: Check diff ns like dublin
        comments,
        guid
    };

public slots:
    Q_INVOKABLE bool isValid() const;
    Q_INVOKABLE void setSource(QIODevice* xmlSource);
    Q_INVOKABLE QIODevice* source() const;
    Q_INVOKABLE QUrl imageUrl();
    Q_INVOKABLE QString channelElement(RSSElement aElement);
    Q_INVOKABLE QString channelElement(QString userElement);
    Q_INVOKABLE QString itemElement(int itemIndex,RSSElement aElement);
    Q_INVOKABLE QString itemElement(int itemIndex,QString userElement);
    Q_INVOKABLE QStringList itemElements(RSSElement aElement);
    Q_INVOKABLE QStringList itemElements(QString userElement);
    Q_INVOKABLE QStringList category(int itemIndex);
    Q_INVOKABLE QList<QStringList> categories();
    Q_INVOKABLE int count();
    void invalidateCache();
    bool isError() const;
    static QString decodeHtml(const QString& encodedHtml);

public slots:
    QString executeQuery(const QString& aQuery);
    QStringList executeQueryAsList(const QString& aQuery);
    QString enumToString(RSSParser::RSSElement aElement);

private:
    QXmlQuery m_xmlQuery;
    QIODevice* m_xmlSource;
    QString m_xmlSourceFileName;
    bool m_IsError;
    int m_Count;
    int m_InvalidateCachedCount;
};

#endif // RSSPARSER_H
