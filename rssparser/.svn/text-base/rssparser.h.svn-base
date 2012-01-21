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
    Q_INVOKABLE bool isValid();
    Q_INVOKABLE void setSource(QIODevice* xmlSource);
    Q_INVOKABLE bool setSourceFileName(QString sourceFileName);
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
    bool isError();
    static QString decodeHtml(const QString& encodedHtml);
    // TODO: add method to query if an element exists

//private slots:
public slots:
    QString executeQuery(const QString& aQuery);
    QStringList executeQueryAsList(const QString& aQuery);
    QString enumToString(RSSParser::RSSElement aElement);

private:
    QXmlQuery m_xmlQuery;
    QIODevice* m_xmlSource;
    QString m_xmlSourceFileName;
    bool m_IsError;
};

#endif // RSSPARSER_H
