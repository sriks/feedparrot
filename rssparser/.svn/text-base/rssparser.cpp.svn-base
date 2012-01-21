#include <QIODevice>
#include <QUrl>
#include <QMetaEnum>
#include <QMetaObject>
#include <QDebug>
#include <QXmlItem>
#include "rssparser.h"

// FIXME: handle encoding properly
// XQueries
//
// Get channel image url
const QString KXqChannelImageUrl("let $image := doc($xmlSource)//channel/image return (data($image/*:url))");
// Query an element in channel other than image
const QString KXqChannelElementQuery("let $channel := doc($xmlSource)//channel return (data($channel/*:%1))");
// Query Item count
const QString KXqItemCount("let $x := doc($xmlSource)//item return count($x)");
// Query element of an item identified with index
const QString KXqItemQuery("let $item := doc($xmlSource)//item[%1] return (data($item/*:%2))");
// Query all item elements
// fn:string is required by QXmlQuery to evaluate to QSrtringList
// fn:string requires index, so running a for loop.
// TODO: can this query be optimized?
const QString KXqAllItemElementsQuery("for $root in doc($xmlSource)//channel/item return string($root/*:%1)");

// Query categories in an item
const QString KXqItemCategories("let $category := doc($xmlSource)//channel/item[%1]/*:category for $categorylist in $category return string($categorylist)");
const QString KXmlSource("xmlSource");

/*!
  \class RSSParser
  \brief RSSParser is a special xml parser for RSS feeds. It is based on XQuery as backend.
  It is very light weight as it does not own the source data. Using RSSParser requires some basic knowledge of RSS elements.
  It suports RSS Specification http://www.rssboard.org/rss-specification

  \section source Setting source
  RSS Source can be set using \ref setSource.

  \code
    QRSSParser parser;
    QFile sourceFile("rss.xml"); // file in which rss feed is collected/stored.
    if(source.open(QIODevice::ReadOnly))
    {
       // set source
       parser.setSource(sourceFile);

       // use parser
    }
    sourceFile.close();
  \endcode

  \n or

  \ref QBuffer can also be used in \ref setSource.
  \code
    QString sourceXml;
    QBuffer buffer(sourceXml.toUtf8());
    if(buffer.open(QIODevice::ReadOnly)) // ensure buffer is opened in read mode
    {
        parser.setSource(&buffer);
        if(parser.isValid())
        {
        // use parser
        }
    }
    buffer.close();
  \endcode

  \section usage Usage
  Usage of RSSParser is designed around RSS Specification http://www.rssboard.org/rss-specification
  \li \ref channelElement is used to query channel elements
  \li \ref itemElement is used to query item elements
  \li some special methods like \ref imageUrl, \ref itemCount, \ref category, \ref categories can be used to query other elements.
  \n RSS elements can be specified using \ref RSSParser::RSSElement
  \code
    QRSSParser parser;
    QFile sourceFile("rss.xml"); // file in which rss feed is collected/stored.
    if(source.open(QIODevice::ReadOnly))
    {
       // set source
       parser.setSource(sourceFile);

       if(parser.isValid())
       {
       // get name of channel
       QString channelname = parser.channelElement(RSSParser::title);

       // get channel image
       QUrl imageUrl = parser.imageUrl();

       // get count of items
       int count = parser.itemCount();

       // get title of first item
       QString title = parser.itemElement(1,RSSParser::title); // XQuery numbering starts from 1
       }
    }
    sourceFile.close();
  \endcode

  \section error Error Handling
   Parsing error in the backend (XQuery) can be found using \ref isError.

   \code
    if(parser.isValid())
    {
        // querying out of bounds item
        QString title = parser.itemElement(35,RSSParser::title);

        if(parser.isError())
        {
            // some error in request.
        }
    }
   \endcode
   \ref isError is reflects error state of the latest request only.
  */

RSSParser::RSSParser(QObject *parent) :
    QObject(parent)
{
    m_xmlSource = NULL;
    m_IsError = false;
}

RSSParser::~RSSParser()
{
    qDebug()<<__FUNCTION__;
}

/**
  \brief Validates parser.
  Returns true if parser is valid.
  Parser is valid if a readable non empty source is set using \ref setSource
  \return true if valid
  **/
bool RSSParser::isValid()
{
    if(m_xmlSource)
    {
    return ( (m_xmlSource->isReadable()) &&
             (m_xmlSource->size()) );
    }

    else
    {
    return !m_xmlSourceFileName.isEmpty();
    }
}

/*! \brief set RSS data source.
    Sets xml source to be parsed using RSSParser.
    The source should be opened atleast in \ref QIODevice::ReadOnly and should be non empty.
    use \ref isValid() to validate the parser after setting source
    \param xmlSource source of xml
*/
void RSSParser::setSource(QIODevice* xmlSource)
{
      m_xmlSource = xmlSource;
      m_xmlQuery.bindVariable(KXmlSource,xmlSource);
}

/*! \brief Experimental API to set a filename as source. */
bool RSSParser::setSourceFileName(QString sourceFileName)
{
    if(sourceFileName.isEmpty())
    {return false;}
    m_xmlSourceFileName = sourceFileName;
    m_xmlQuery.bindVariable(KXmlSource,QXmlItem(QVariant(m_xmlSourceFileName)));
    m_xmlSource = NULL;
    return true;
}

/*! \brief returns URL of channel image  */
QUrl RSSParser::imageUrl()
{
    return QUrl(executeQuery(KXqChannelImageUrl));
}

/*! \brief queries a channel element
    \param element Element of intrest
    \return If no such element exist returns empty QString.
    \sa isError()
*/
QString RSSParser::channelElement(RSSElement element)
{
    QString enumString = enumToString(element);
    return executeQuery(KXqChannelElementQuery.arg(enumString));
}

/*! \brief queries a channel element provided as a string
    \param userElement Element of intrest
    \return If no such element exist returns empty QString.
    \sa isError()
*/
QString RSSParser::channelElement(QString userElement)
{
    return executeQuery(KXqChannelElementQuery.arg(userElement));
}

/*! \brief queries an item element
    \param itemIndex Index of the item in the feed
    \attention In XQuery item count starts from 1.
    \param element Element of intrest
    \return If no such element exist returns empty QString.
    \sa itemElements(), isError()
*/
QString RSSParser::itemElement(int itemIndex,RSSElement element)
{
    QString enumString = enumToString(element);
    return executeQuery(KXqItemQuery.arg(itemIndex).arg(enumString));
}

/*! \brief queries an item element provided as string
    \param itemIndex Index of the item in the feed
    \attention In XQuery item count starts from 1.
    \param userElement Element of intrest
    \return If no such element exist returns empty QString.
    \sa itemElements(), isError()
*/
QString RSSParser::itemElement(int itemIndex,QString userElement)
{
    return executeQuery(KXqItemQuery.arg(itemIndex).arg(userElement));
}

/*! \brief Convinience method to query all items for the element
    \param element Element of intrest
    \return If no such element exist returns empty QStringList.

    For example to query all titles in the feed
    \code
    QStringList allTitles = parser.itemElements(RSSParser::title);
    \endcode

    For categories use special method \ref category() \ref categories()
    \sa itemElement(), itemCount(), isError()
 */
QStringList RSSParser::itemElements(RSSElement element)
{
    QString enumString = enumToString(element);
    return executeQueryAsList(KXqAllItemElementsQuery.arg(enumString));
}

QStringList RSSParser::itemElements(QString userElement)
{
    return executeQueryAsList(KXqAllItemElementsQuery.arg(userElement));
}

/*! \brief Returns categories of a particular item in feed
    \param itemIndex Index of intrest
    \attention In XQuery item count starts from 1.
    \return returns empty QStringList if no such item exists or in case of error
    \sa isError(), categories()
*/
QStringList RSSParser::category(int itemIndex)
{
    // Query all items
    if(itemIndex)
    {
    return executeQueryAsList(KXqItemCategories.arg(QString().number(itemIndex)));
    }
return QStringList();
}

/*! \brief Convinience method to query categories of all items.
    \return returns empty list in case of error.
    \sa isError(), category()
*/
QList<QStringList> RSSParser::categories()
{
    QList<QStringList> list;
    int itemcount = count();
    for(int i=1;i<=itemcount;i++) // XQuery indexing starts with 1
    {
        list<<category(i);
    }
    return list;
}

/*! \brief Returns count of all items in the feed.
    \attention In XQuery item count starts from 1.
    \return count of items
    \sa isError()
*/
int RSSParser::count()
{
    return executeQuery(KXqItemCount).toInt();
}

/*! \brief returns true if the latest query resulted in error.*/
bool RSSParser::isError()
{
    return m_IsError;
}

// Internal API
QString RSSParser::executeQuery(const QString& aQuery)
{
    m_IsError = false;
    m_xmlQuery.setQuery(aQuery);
    QString result = QString();

    if(m_xmlQuery.isValid())
    {
       m_xmlQuery.evaluateTo(&result);
    }

    else
    {
        m_IsError = true;
        qWarning()<<__FUNCTION__<<" invalid query \n"<<aQuery;
    }
    return result;
}

// Internal API
// Executes a xquery that returns as a sequence of sting list
QStringList RSSParser::executeQueryAsList(const QString& aQuery)
{
    m_IsError = false;
    m_xmlQuery.setQuery(aQuery);
    QStringList result;
    result.clear();

    if(m_xmlQuery.isValid())
    {
       m_xmlQuery.evaluateTo(&result);
    }

    else
    {
        m_IsError = true;
        qWarning()<<__FUNCTION__<<" invalid query \n"<<aQuery;
    }
    return result;
}

// Internal API
QString RSSParser::enumToString(RSSParser::RSSElement aElement)
{
    int index = metaObject()->indexOfEnumerator("RSSElement");
    QMetaEnum menum = metaObject()->enumerator(index);
    return menum.valueToKey(aElement);
}

// Internal API
// Decodes HTML encoded string.
QString RSSParser::decodeHtml(const QString& encodedHtml)
{
    QString temp = encodedHtml;
    temp.replace("&amp;", "&");
    temp.replace("&apos;", "'");
    temp.replace("&quot;", "\"");
    temp.replace("&lt;", "<");
    temp.replace("&gt;", ">");
return temp;
}
// end of file
