#include <QtGui/QApplication>
#include <QWebView>
#include <QUrl>
#include <QDebug>
#include <QFile>
#include <QBuffer>
#include <QXmlQuery>
#include <QTextDocument>
#include "mainwindow.h"
#include "locationengine.h"
#include "rssparser.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    LocationEngine locationEngine;
//    locationEngine.getRss();

    QWebView view;

    QFile file(":/xml/engadget.xml");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray tmpData = file.readAll();
        // Set data source
        QBuffer buffer(&tmpData);
        qDebug()<<"Buffer open:"<<buffer.open(QIODevice::ReadOnly);
        RSSParser rss;
        rss.setSource(&buffer);
        qDebug()<<rss.channelElement(RSSParser::description);
        qDebug()<<"item count:"<<rss.itemCount();
//        /for(int i=0;i<rss.itemCount();i++)
        {
            qDebug()<<rss.itemElement(1,RSSParser::title);
            QString des = rss.itemElement(1,RSSParser::description);

            QUrl url(des);
            QByteArray per = url.toPercentEncoding(des);
            qDebug()<<per;
            qDebug()<<"\n\n\n"<<url.fromPercentEncoding(per);
            view.setHtml(des);

        view.show();
        }

        qDebug()<<rss.imageUrl().toString();
    }

    else
    {
        qDebug()<<"failed to open";
    }

    MainWindow w;
    w.show();
    return a.exec();
}

