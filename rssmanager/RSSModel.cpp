
/*
    RSSModel.cpp
*/

#include <QtGui>
#include <QDebug>
#include <QModelIndex>
#include <QVariant>

//#include "treeitem.h"
#include "RSSModel.h"
#include "RSSManager.h"
#include "feedprofile.h"

const int ROOT = -10;
const int FEED = -11;

struct RSSModelPrivate {
    RSSManager* manager; // non-owning
};

RSSModel::RSSModel(RSSManager* manager, QObject *parent)
    : QAbstractItemModel(parent)
{
    d = new RSSModelPrivate;
    d->manager = manager;
}

RSSModel::~RSSModel() {
    delete d;
}

int RSSModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant RSSModel::data(const QModelIndex &index, int role) const
{
//    qDebug()<<__PRETTY_FUNCTION__;
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    QString text("testdata");
//    if(index.parent().isValid()) {
//        int parentRow = index.parent().row();
//        RSSParser* p = d->manager->feedProfile(parentRow)->parser();
//        text = p->itemElement(index.row(),RSSParser::title);
//    }
//    else {
//        text = d->manager->feedUrls().at(index.row()).toString();
//    }
    return text;
}

Qt::ItemFlags RSSModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant RSSModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
//    qDebug()<<__PRETTY_FUNCTION__<<"section:"<<section;
    return QVariant();
}

QModelIndex RSSModel::index(int row, int column, const QModelIndex &parent) const {

    return createIndex(1,0,0);
    if (!hasIndex(row, column, parent)) {
        qDebug()<<__PRETTY_FUNCTION__<<" parent internal id:"<<parent.internalId();
        return QModelIndex();
    }

    if(!parent.isValid()) {
        qDebug()<<__PRETTY_FUNCTION__<<" !!parent internal id:"<<parent.internalId();
    }


    int id;
    // root
    if(!parent.isValid()) {
        //id = ROOT;
        return createIndex(row,column,ROOT);
    }
    else {
        id = parent.internalId();
    }

    if(ROOT == id) {
        return createIndex(row,column,FEED);
    }
    else if(FEED == id) {
        FeedProfile* fp = d->manager->feedProfile(parent.row());
        return createIndex(row,column,fp);
    }

    return QModelIndex();
}

QModelIndex RSSModel::parent(const QModelIndex &index) const
{
    qDebug()<<__PRETTY_FUNCTION__;

    if (!index.isValid())
        return QModelIndex();

    int id = index.internalId();
    if(ROOT == id) {
        return QModelIndex();
    }
    else if(FEED == id) {
        return createIndex(0,0,ROOT);
    }
    else {
        //return createIndex(id,0,FEED);
        qDebug()<<__PRETTY_FUNCTION__<<" checking for feed item";
        FeedProfile* fp = static_cast<FeedProfile*>(index.internalPointer());
        if(fp) {
           QString feedurl = fp->subscription().sourceUrl().toString();
           int parentRow = d->manager->feedUrls().indexOf(feedurl);
           qDebug()<<__PRETTY_FUNCTION__<<"src:"<<feedurl<<" row:"<<parentRow;
           return createIndex(parentRow,0,FEED);
        }
    }
//    else
//        return QModelIndex();

//    if (!index.isValid())
//        return QModelIndex();

//    int id = index.internalId();
//    qDebug()<<"id:"<<id;
//    qDebug()<<"row:"<<index.row();
//    // top level item
//    if(-1 == id) {
//        return QModelIndex();
//    }
//    // tree item
//    else {
//        return createIndex(id,0,-1);
////        return QModelIndex();
//    }
////    return QModelIndex();
}

int RSSModel::rowCount(const QModelIndex &parent) const
{
//    qDebug()<<__PRETTY_FUNCTION__;
    int rowcount = 0;
    if(parent.isValid()) {
        int parentRow = parent.row();
        FeedProfile* fp = d->manager->feedProfile(parentRow);
        if(fp) {
            rowcount = fp->count();
        }
    }

    // top level item
    else {
        rowcount = d->manager->feedsCount();
    }
return rowcount;

}

void RSSModel::updateView() {
    this->beginInsertRows(QModelIndex(),0,this->rowCount());
    this->endInsertRows();
}

// eof
