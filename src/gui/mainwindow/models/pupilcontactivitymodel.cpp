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

#include "pupilcontactivitymodel.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"
#include "configfile.h"

PupilContActivityModel::PupilContActivityModel(QObject *parent)
    : QSqlQueryModel(parent)
{
}

Qt::ItemFlags PupilContActivityModel::flags(
    const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if (index.column() > 0 )
        flags |= Qt::ItemIsEditable;
    return flags;
}

bool PupilContActivityModel::setData(const QModelIndex &index, const QVariant &value, int /* role */)
{
    if (index.column() < 1 || index.column() > 6)
        return false;

    QModelIndex activityIdIndex = QSqlQueryModel::index(index.row(), 0);
    int actvitiyId = data(activityIdIndex,0).toInt();

    clear();

    bool ok = true;
    switch (index.column()) {
    case 1:
        ok = setDesc(actvitiyId, value.toString());
        break;
    case 2:
        ok = setType(actvitiyId, value.toInt());
        break;
    case 3:
        ok = setDay(actvitiyId, value.toInt());
        break;
    case 4:
        ok = setTime(actvitiyId, value.toString());
        break;
    case 5:
        ok = setStartDate(actvitiyId, value.toString());
        break;
    case 6:
        ok = setStopDate(actvitiyId, value.toString());
        break;
    }
    refresh();
    return ok;
}

QVariant PupilContActivityModel::data(const QModelIndex &idx, int role) const
{
    QVariant value = QSqlQueryModel::data(idx, role);

    QStringList dayList;
    dayList << tr("Montag") << tr("Dienstag") << tr("Mittwoch") << tr("Donnerstag") << tr("Freitag") << tr("Samstag") << tr("Sonntag") << QString::fromUtf8(tr("unregelmäßig").toStdString().c_str());

    QStringList typeList;
    typeList << tr("Ensemble") << tr("Theorieunterricht") << tr("Korrepetition") << tr("Sonstiges");

    if (value.isValid() && role == Qt::DisplayRole) {

        if (idx.column() == 2) {
            return typeList.at(value.toInt());
        }
        if (idx.column() == 3) {
            return dayList.at(value.toInt());
        }
    }

    return value;
}

void PupilContActivityModel::refresh()
{
    setQuery("SELECT activityid, desc, continoustype, continousday, continoustime, strftime(\"%d.%m.%Y\", date), strftime(\"%d.%m.%Y\", continousstopdate) FROM activity WHERE pupilid="+QString::number(currentPupilId,10)+" AND ifcontinous=1 ORDER BY date DESC", *myW->getMyDb()->getMyPupilDb());
    setHeaderData(1, Qt::Horizontal, QObject::tr("Bezeichnung"));
    setHeaderData(2, Qt::Horizontal, QObject::tr("Typ"));
    setHeaderData(3, Qt::Horizontal, QObject::tr("Wochentag"));
    setHeaderData(4, Qt::Horizontal, QObject::tr("Uhrzeit   "));
    setHeaderData(5, Qt::Horizontal, QObject::tr("Datum (Beginn)"));
    setHeaderData(6, Qt::Horizontal, QObject::tr("Datum (Ende)"));
}

bool PupilContActivityModel::setStartDate(int activityId, const QString &date)
{
    QSqlQuery query;
    query.prepare("UPDATE activity SET date = ? where activityid = ?");
    query.addBindValue(date);
    query.addBindValue(activityId);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 81 - " << query.lastError();
        return false;
    } else return true;
}

bool PupilContActivityModel::setStopDate(int activityId, const QString &date)
{
    QSqlQuery query;
    query.prepare("UPDATE activity SET continousstopdate = ? where activityid = ?");
    query.addBindValue(date);
    query.addBindValue(activityId);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 82 - " << query.lastError();
        return false;
    } else return true;
}

bool PupilContActivityModel::setDesc(int activityId, const QString &desc)
{
    QSqlQuery query;
    query.prepare("UPDATE activity SET desc = ? where activityid = ?");
    query.addBindValue(desc);
    query.addBindValue(activityId);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 83 - " << query.lastError();
        return false;
    } else return true;
}

bool PupilContActivityModel::setDay(int activityId, const int &day)
{
    QSqlQuery query;
    query.prepare("UPDATE activity SET continousday = ? where activityid = ?");
    query.addBindValue(day);
    query.addBindValue(activityId);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 84 - " << query.lastError();
        return false;
    } else return true;
}

bool PupilContActivityModel::setType(int activityId, const int &type)
{
    QSqlQuery query;
    query.prepare("UPDATE activity SET continoustype = ? where activityid = ?");
    query.addBindValue(type);
    query.addBindValue(activityId);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 255 - " << query.lastError();
        return false;
    } else return true;
}


bool PupilContActivityModel::setTime(int activityId, const QString &time)
{
    QSqlQuery query;
    query.prepare("UPDATE activity SET continoustime = ? where activityid = ?");
    query.addBindValue(time);
    query.addBindValue(activityId);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 85 - " << query.lastError();
        return false;
    } else return true;
}
