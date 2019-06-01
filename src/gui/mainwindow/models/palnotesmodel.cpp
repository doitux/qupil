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

#include "palnotesmodel.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"
#include "configfile.h"

PalNotesModel::PalNotesModel(QObject *parent)
    : QSqlQueryModel(parent)
{
}

Qt::ItemFlags PalNotesModel::flags(
    const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if (index.column() == 2 || index.column() == 3)
        flags |= Qt::ItemIsEditable;
    return flags;
}

bool PalNotesModel::setData(const QModelIndex &index, const QVariant &value, int /* role */)
{
    if (index.column() < 2 || index.column() > 3)
        return false;

    QModelIndex noteIdIndex = QSqlQueryModel::index(index.row(), 0);
    QModelIndex cnoteIdIndex = QSqlQueryModel::index(index.row(), 1);
    int noteId = data(noteIdIndex,0).toInt();
    int cnoteId  = data(cnoteIdIndex,0).toInt();

    clear();

    bool ok;
    if (index.column() == 2) {
        ok = setDate(noteId, cnoteId, value.toString());
    } else {
        ok = setContent(noteId, cnoteId, value.toString());
    }
    refresh();
    return ok;
}

QVariant PalNotesModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);

    if (role == Qt::TextColorRole && index.column() == 2)
        return qVariantFromValue(QColor(Qt::blue));

    return value;
}

void PalNotesModel::refresh()
{
    if(myConfig->readConfigInt("LimitLoadLessonNotes")) {
        setQuery("SELECT n.noteid, n.cnoteid, strftime(\"%d.%m.%Y\", n.date), n.content FROM note n, pupilatlesson pal WHERE pal.palid="+QString::number(currentPalId,10)+" AND pal.palid=n.palid AND pal.startdate <= n.date AND pal.stopdate >= n.date ORDER BY n.date DESC LIMIT " + QString::number(myConfig->readConfigInt("LoadLessonNotesNumber")), *myW->getMyDb()->getMyPupilDb());
    }
    else {
        setQuery("SELECT n.noteid, n.cnoteid, strftime(\"%d.%m.%Y\", n.date), n.content FROM note n, pupilatlesson pal WHERE pal.palid="+QString::number(currentPalId,10)+" AND pal.palid=n.palid AND pal.startdate <= n.date AND pal.stopdate >= n.date ORDER BY n.date DESC", *myW->getMyDb()->getMyPupilDb());
    }
    setHeaderData(2, Qt::Horizontal, tr("Date"));
    setHeaderData(3, Qt::Horizontal, tr("Note"));
}

bool PalNotesModel::setDate(int noteId, int cnoteId, const QString &date)
{
    if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {
        QSqlQuery query;
        query.prepare("UPDATE note SET date = ? where cnoteid = ?");
        query.addBindValue(date);
        query.addBindValue(cnoteId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 64 - " << query.lastError();
            return false;
        } else return true;
    } else {
        QSqlQuery query;
        query.prepare("UPDATE note SET date = ? where noteid = ?");
        query.addBindValue(date);
        query.addBindValue(noteId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 65 - " << query.lastError();
            return false;
        } else return true;
    }
}

bool PalNotesModel::setContent(int noteId, int cnoteId, const QString &content)
{
    if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {
        QSqlQuery query;
        query.prepare("UPDATE note SET content = ? where cnoteid = ?");
        query.addBindValue(content);
        query.addBindValue(cnoteId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 66 - " << query.lastError();
            return false;
        } else return true;
    } else {
        QSqlQuery query;
        query.prepare("UPDATE note SET content = ? where noteid = ?");
        query.addBindValue(content);
        query.addBindValue(noteId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 67 - " << query.lastError();
            return false;
        } else return true;
    }
}
