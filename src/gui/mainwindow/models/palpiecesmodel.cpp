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

#include "palpiecesmodel.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"
#include "configfile.h"

PalPiecesModel::PalPiecesModel(QObject *parent)
    : QSqlQueryModel(parent)
{
}

Qt::ItemFlags PalPiecesModel::flags(
    const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if (index.column() > 0 )
        flags |= Qt::ItemIsEditable;
    return flags;
}

bool PalPiecesModel::setData(const QModelIndex &index, const QVariant &value, int /* role */)
{
    if (index.column() < 2 || index.column() > 8)
        return false;

    QModelIndex pieceIdIndex = QSqlQueryModel::index(index.row(), 0);
    QModelIndex cpieceIdIndex = QSqlQueryModel::index(index.row(), 1);
    int pieceId = data(pieceIdIndex,0).toInt();
    int cpieceId  = data(cpieceIdIndex,0).toInt();

    clear();

    bool ok = true;
    switch (index.column()) {
    case 2:
        ok = setComposer(pieceId, cpieceId, value.toString());
        break;
    case 3:
        ok = setTitle(pieceId, cpieceId, value.toString());
        break;
    case 4:
        ok = setGenre(pieceId, cpieceId, value.toString());
        break;
    case 5:
        ok = setDuration(pieceId, cpieceId, value.toInt());
        break;
    case 6:
        ok = setStartDate(pieceId, cpieceId, value.toString());
        break;
    case 7:
        ok = setStopDate(pieceId, cpieceId, value.toString());
        break;
    case 8:
        ok = setState(pieceId, cpieceId, value.toInt());
        break;
    }
    refresh();
    return ok;
}

QVariant PalPiecesModel::data(const QModelIndex &idx, int role) const
{
    QVariant value = QSqlQueryModel::data(idx, role);

    QStringList myStateStringList;
    myStateStringList << tr("Planned") << tr("In Progress") << tr("Paused") << tr("Ready for Concert") << tr("Finished");

    if ((role == Qt::BackgroundRole) && (index(idx.row(), 8, idx.parent()).data().toString() == myStateStringList.at(1))) {
        return QColor(156, 255, 103);
    }
    if ((role == Qt::BackgroundRole) && (index(idx.row(), 8, idx.parent()).data().toString() == myStateStringList.at(2))) {
        return QColor(Qt::yellow);
    }
    if ((role == Qt::BackgroundRole) && (index(idx.row(), 8, idx.parent()).data().toString() == myStateStringList.at(3))) {
        return QColor(255, 128, 128);
    }
    if ((role == Qt::BackgroundRole) && (index(idx.row(), 8, idx.parent()).data().toString() == myStateStringList.at(4))) {
        return QColor(Qt::lightGray);
    }

    if (value.isValid() && role == Qt::DisplayRole) {
        if (idx.column() == 8) {
            return myStateStringList.at(value.toInt());
        }
    }

//      if (role == Qt::TextColorRole && index.column() == 1)
//          return qVariantFromValue(QColor(Qt::blue));

    return value;
}

void PalPiecesModel::refresh()
{
    QSqlQuery query("SELECT p.pieceid, p.cpieceid, pc.composer, p.title, p.genre, p.duration, strftime(\"%d.%m.%Y\", p.startdate), strftime(\"%d.%m.%Y\", p.stopdate), p.state FROM piece p, pupilatlesson pal, piececomposer pc WHERE pal.palid="+QString::number(currentPalId,10)+" AND pal.palid=p.palid AND p.piececomposerid=pc.piececomposerid AND pal.startdate <= p.startdate AND pal.stopdate >= p.startdate ORDER BY p.startdate DESC", *myW->getMyDb()->getMyPupilDb());
    setQuery(query);
    if (query.lastError().isValid()) qDebug() << "DB Error: 68 - " << query.lastError();

    setHeaderData(2, Qt::Horizontal, tr("Composer"));
    setHeaderData(3, Qt::Horizontal, tr("Title"));
    setHeaderData(4, Qt::Horizontal, tr("Genre               "));
    setHeaderData(5, Qt::Horizontal, tr("Duration"));
    setHeaderData(6, Qt::Horizontal, tr("Start"));
    setHeaderData(7, Qt::Horizontal, tr("End"));
    setHeaderData(8, Qt::Horizontal, tr("State"));
}


bool PalPiecesModel::setComposer(int pieceId, int cpieceId, const QString &composer)
{
    if(composer.isEmpty()) {
        return false;
    } else {
        QString authorId;
        QSqlQuery query;
        query.prepare("SELECT piececomposerid FROM piececomposer WHERE composer=?");
        query.addBindValue(composer);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 188 - " << query.lastError();
        }
        if(query.next()) {
            authorId = query.value(0).toString();
        } else {
            query.prepare("INSERT INTO piececomposer (composer) VALUES (?)");
            query.addBindValue(composer);
            query.exec();
            if (query.lastError().isValid()) {
                qDebug() << "DB Error: 189 - " << query.lastError();
            }
            authorId = query.lastInsertId().toString();
        }

        if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {
            QSqlQuery query;
            query.prepare("UPDATE piece SET piececomposerid = ? where cpieceid = ?");
            query.addBindValue(authorId.toInt());
            query.addBindValue(cpieceId);
            query.exec();
            if (query.lastError().isValid()) {
                qDebug() << "DB Error: 190 - " << query.lastError();
                return false;
            } else return true;
        } else {
            QSqlQuery query;
            query.prepare("UPDATE piece SET piececomposerid = ? where pieceid = ?");
            query.addBindValue(authorId.toInt());
            query.addBindValue(pieceId);
            query.exec();
            if (query.lastError().isValid()) {
                qDebug() << "DB Error: 191 - " << query.lastError();
                return false;
            } else return true;
        }
    }
}

bool PalPiecesModel::setTitle(int pieceId, int cpieceId, const QString &title)
{
    if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {
        QSqlQuery query;
        query.prepare("UPDATE piece SET title = ? where cpieceid = ?");
        query.addBindValue(title);
        query.addBindValue(cpieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 69 - " << query.lastError();
            return false;
        } else return true;
    } else {
        QSqlQuery query;
        query.prepare("UPDATE piece SET title = ? where pieceid = ?");
        query.addBindValue(title);
        query.addBindValue(pieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 70 - " << query.lastError();
            return false;
        } else return true;
    }
}

bool PalPiecesModel::setGenre(int pieceId, int cpieceId, const QString &genre)
{

    if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {
        QSqlQuery query;
        query.prepare("UPDATE piece SET genre = ? where cpieceid = ?");
        query.addBindValue(genre);
        query.addBindValue(cpieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 71 - " << query.lastError();
            return false;
        } else return true;
    } else {
        QSqlQuery query;
        query.prepare("UPDATE piece SET genre = ? where pieceid = ?");
        query.addBindValue(genre);
        query.addBindValue(pieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 72 - " << query.lastError();
            return false;
        } else return true;
    }
}

bool PalPiecesModel::setDuration(int pieceId, int cpieceId, const int &duration)
{

    if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {
        QSqlQuery query;
        query.prepare("UPDATE piece SET duration = ? where cpieceid = ?");
        query.addBindValue(duration);
        query.addBindValue(cpieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 73 - " << query.lastError();
            return false;
        } else return true;
    } else {
        QSqlQuery query;
        query.prepare("UPDATE piece SET duration = ? where pieceid = ?");
        query.addBindValue(duration);
        query.addBindValue(pieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 74 - " << query.lastError();
            return false;
        } else return true;
    }
}

bool PalPiecesModel::setStartDate(int pieceId, int cpieceId, const QString &date)
{
    if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {
        QSqlQuery query;
        query.prepare("UPDATE piece SET startdate = ? where cpieceid = ?");
        query.addBindValue(date);
        query.addBindValue(cpieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 75 - " << query.lastError();
            return false;
        } else return true;
    } else {
        QSqlQuery query;
        query.prepare("UPDATE piece SET startdate = ? where pieceid = ?");
        query.addBindValue(date);
        query.addBindValue(pieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 76 - " << query.lastError();
            return false;
        } else return true;
    }
}

bool PalPiecesModel::setStopDate(int pieceId, int cpieceId, const QString &date)
{
    if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {
        QSqlQuery query;
        query.prepare("UPDATE piece SET stopdate = ? where cpieceid = ?");
        query.addBindValue(date);
        query.addBindValue(cpieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 77 - " << query.lastError();
            return false;
        } else return true;
    } else {
        QSqlQuery query;
        query.prepare("UPDATE piece SET stopdate = ? where pieceid = ?");
        query.addBindValue(date);
        query.addBindValue(pieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 78 - " << query.lastError();
            return false;
        } else return true;
    }
}

bool PalPiecesModel::setState(int pieceId, int cpieceId, const int &stateId)
{
    if(stateId==4)
        setStopDate(pieceId, cpieceId, QDate::currentDate().toString((Qt::ISODate)));

    if(myConfig->readConfigInt("SaveNotesPiecesForAllPupil")) {
        QSqlQuery query;
        query.prepare("UPDATE piece SET state = ? where cpieceid = ?");
        query.addBindValue(stateId);
        query.addBindValue(cpieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 79 - " << query.lastError();
            return false;
        } else return true;
    } else {
        QSqlQuery query;
        query.prepare("UPDATE piece SET state = ? where pieceid = ?");
        query.addBindValue(stateId);
        query.addBindValue(pieceId);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "DB Error: 80 - " << query.lastError();
            return false;
        } else return true;
    }
}
