/***************************************************************************
 *   Copyright (C) 2008 by Felix Hammer   *
 *   f.hammer@gmx.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QtSql>

#include "smlallmodel.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"
#include "configfile.h"

SmlAllModel::SmlAllModel(QObject *parent)
    : QSqlQueryModel(parent)
{
}

Qt::ItemFlags SmlAllModel::flags(
    const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if (index.column() > 0 )
        flags |= Qt::ItemIsEditable;
    return flags;
}

bool SmlAllModel::setData(const QModelIndex &index, const QVariant &value, int /* role */)
{
    if (index.column() == 0)
        return false;

    QModelIndex smlIdIndex = QSqlQueryModel::index(index.row(), 0);
    int smlId = data(smlIdIndex,0).toInt();

    clear();

    bool ok = TRUE;
    switch (index.column()) {
    case 1:
        ok = setAuthor(smlId, value.toString());
        break;
    case 2:
        ok = setTitle(smlId, value.toString());
        break;
    case 3:
        ok = setPublisher(smlId, value.toString());
        break;
    }
    refresh();
    return ok;
}

QVariant SmlAllModel::data(const QModelIndex &idx, int role) const
{
    QVariant value = QSqlQueryModel::data(idx, role);

    if (role == Qt::DisplayRole && idx.column() == 0) {
        QString id(value.toString());
        int length = id.length();
        int i;
        for(i=length; i < 4; i++) id.prepend("0");
        return id;
    }

    return value;
}

void SmlAllModel::refresh(int orderColumn, int orderMode)
{
    QString orderString("");
    switch (orderColumn) {
    case 0: {
        if(orderMode) orderString = "ORDER by sml.smlid DESC";
        else orderString = "ORDER by sml.smlid ASC";
    }
    break;
    case 1: {
        if(orderMode) orderString = "ORDER by smla.author DESC";
        else orderString = "ORDER by smla.author ASC";
    }
    break;
    case 2: {
        if(orderMode) orderString = "ORDER by sml.title DESC";
        else orderString = "ORDER by sml.title ASC";
    }
    break;
    case 3: {
        if(orderMode) orderString = "ORDER by smlp.publisher DESC";
        else orderString = "ORDER by smlp.publisher ASC";
    }
    break;
    }

    QSqlQuery query("SELECT sml.smlid, smla.author, sml.title, smlp.publisher FROM sheetmusiclibrary sml, smlauthor smla, smlpublisher smlp WHERE smla.smlauthorid=sml.author AND smlp.smlpublisherid=sml.publisher "+orderString, *myW->getMyDb()->getMyPupilDb());
    setQuery(query);
    if (query.lastError().isValid()) qDebug() << "DB Error: 148 - " << query.lastError();

    setHeaderData(0, Qt::Horizontal, QObject::tr("Index"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("Komponist / Author"));
    setHeaderData(2, Qt::Horizontal, QObject::tr("Titel"));
    setHeaderData(3, Qt::Horizontal, QObject::tr("Verlag"));
}

bool SmlAllModel::setTitle(int smlId, const QString &title)
{
    QSqlQuery query;
    query.prepare("UPDATE sheetmusiclibrary SET title = ? where smlid = ?");
    query.addBindValue(title);
    query.addBindValue(smlId);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 149 - " << query.lastError();
        return FALSE;
    } else return TRUE;


}

bool SmlAllModel::setAuthor(int smlId, const QString &author)
{
    QString authorId;
    QSqlQuery query("SELECT smlauthorid FROM smlauthor WHERE author='"+author+"'");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 150 - " << query.lastError();
    }
    if(query.next()) {
        authorId = query.value(0).toString();
    } else {
        QSqlQuery query1;
        query1.prepare("INSERT INTO smlauthor (author) VALUES (?)");
        query1.addBindValue(author);
        query1.exec();
        if (query1.lastError().isValid()) {
            qDebug() << "DB Error: 151 - " << query1.lastError();
        }
        authorId = query1.lastInsertId().toString();
    }

    QSqlQuery query1;
    query1.prepare("UPDATE sheetmusiclibrary SET author = ? where smlid = ?");
    query1.addBindValue(authorId);
    query1.addBindValue(smlId);
    query1.exec();
    if (query1.lastError().isValid()) {
        qDebug() << "DB Error: 152 - " << query1.lastError();
        return FALSE;
    } else return TRUE;
}

bool SmlAllModel::setPublisher(int smlId, const QString &publisher)
{
    QString publisherId;
    QSqlQuery query2("SELECT smlpublisherid FROM smlpublisher WHERE publisher='"+publisher+"'");
    if (query2.lastError().isValid()) {
        qDebug() << "DB Error: 153 - " << query2.lastError();
    }
    if(query2.next()) {
        publisherId = query2.value(0).toString();
    } else {
        QSqlQuery query3;
        query3.prepare("INSERT INTO smlpublisher (publisher) VALUES (?)");
        query3.addBindValue(publisher);
        query3.exec();
        if (query3.lastError().isValid()) {
            qDebug() << "DB Error: 154 - " << query3.lastError();
        }
        publisherId = query3.lastInsertId().toString();
    }

    QSqlQuery query;
    query.prepare("UPDATE sheetmusiclibrary SET publisher = ? where smlid = ?");
    query.addBindValue(publisherId);
    query.addBindValue(smlId);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 155 - " << query.lastError();
        return FALSE;
    } else return TRUE;
}

void SmlAllModel::sort(int column, Qt::SortOrder order)
{
    refresh(column, order);
}
