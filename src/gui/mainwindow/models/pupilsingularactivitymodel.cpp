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

#include "pupilsingularactivitymodel.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"
#include "configfile.h"

PupilSingularActivityModel::PupilSingularActivityModel(QObject *parent)
    : QSqlQueryModel(parent)
{
}

Qt::ItemFlags PupilSingularActivityModel::flags(
    const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if (index.column() == 1 || index.column() == 2 || index.column() == 3)
        flags |= Qt::ItemIsEditable;
    return flags;
}

bool PupilSingularActivityModel::setData(const QModelIndex &index, const QVariant &value, int /* role */)
{
    if (index.column() < 1 || index.column() > 3)
        return false;

    QModelIndex activityIdIndex = QSqlQueryModel::index(index.row(), 0);
    int actvitiyId = data(activityIdIndex,0).toInt();

    clear();

    bool ok = false;
    if (index.column() == 1) {
        ok = setDesc(actvitiyId, value.toString());
    } else if(index.column() == 2) {
        ok = setDate(actvitiyId, value.toString());
    } else if(index.column() == 3) {
        ok = setType(actvitiyId, value.toString());
    }
    refresh();
    return ok;
}

QVariant PupilSingularActivityModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);

    if (role == Qt::TextColorRole && index.column() == 2)
        return qVariantFromValue(QColor(Qt::blue));

    return value;
}

void PupilSingularActivityModel::refresh()
{
    QStringList myTypeList;
    myTypeList << "Solo-Vortrag" << "Ensemble-Mitwirkung" << "sonstiges";

    setQuery("SELECT activityid, desc, strftime(\"%d.%m.%Y\", date), CASE noncontinoustype WHEN 0 THEN '"+myTypeList.at(0)+"' WHEN 1 THEN '"+myTypeList.at(1)+"' WHEN 2 THEN '"+myTypeList.at(2)+"' END FROM activity WHERE pupilid="+QString::number(currentPupilId,10)+" AND ifcontinous=0 ORDER BY date DESC", *myW->getMyDb()->getMyPupilDb());
// 	removeColumn(0);
    setHeaderData(1, Qt::Horizontal, QObject::tr("Bezeichnung"));
    setHeaderData(2, Qt::Horizontal, QObject::tr("Datum"));
    setHeaderData(3, Qt::Horizontal, QObject::tr("Typ"));
}

bool PupilSingularActivityModel::setDate(int activityId, const QString &date)
{
    QSqlQuery query;
    query.prepare("UPDATE activity SET date = ? where activityid = ?");
    query.addBindValue(date);
    query.addBindValue(activityId);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 86 - " << query.lastError();
        return FALSE;
    } else return TRUE;
}

bool PupilSingularActivityModel::setDesc(int activityId, const QString &desc)
{
    QSqlQuery query;
    query.prepare("UPDATE activity SET desc = ? where activityid = ?");
    query.addBindValue(desc);
    query.addBindValue(activityId);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 87 - " << query.lastError();
        return FALSE;
    } else return TRUE;
}

bool PupilSingularActivityModel::setType(int activityId, const QString &type)
{
    QSqlQuery query;
    query.prepare("UPDATE activity SET noncontinoustype = ? where activityid = ?");
    query.addBindValue(type);
    query.addBindValue(activityId);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 208 - " << query.lastError();
        return FALSE;
    } else return TRUE;
}
