#ifndef TEST_H
#define TEST_H

#include <QObject>

class test:public QObject {
    Q_OBJECT
public:
    test(){
    m = new RSSManager(this);
    model = new RSSModel(m,this);
    v = new QTreeView;
    m->addFeed(FeedSubscription(QUrl("http://www.engadget.com/rss.xml"),15));
    m->startAll();
    connect(m,SIGNAL(updateAvailable(QUrl,int)),this,SLOT(showView()));
    }

public slots:
    void showView() {
        v->setModel(model);
        v->show();
    }

private:
    QTreeView* v;
    RSSManager* m;
    RSSModel* model;
};

#endif // TEST_H
