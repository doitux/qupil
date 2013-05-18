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
#include "dbupdater.h"

#include <iostream>
#include <QtCore>
#include <QtSql>
#include "configfile.h"
#include "qupil_defs.h"

using namespace std;

DBUpdater::DBUpdater(ConfigFile *c, QSqlDatabase *d)
    : myConfig(c), myDB(d)
{

}

DBUpdater::~DBUpdater()
{
}

bool DBUpdater::update(int oldVersion, int newVersion)
{

    qDebug() << "Update data structur from: " << oldVersion << " to: " << newVersion;
    if(newVersion == 2) {
        return update_1_2();
    }

    return false;
}


bool DBUpdater::checkIfDbUpdateNeeded()
{
    QSqlQuery query(*myDB);
    if(!query.exec("SELECT data_structure_rev FROM dbinfos")) {
        qDebug() << "DB Error: 233 - "  << query.lastError();
    } else {
        query.next();
        if(query.value(0).toInt() < THIS_VERSION_DB_DATA_STRUCTURE ) {
            return TRUE;
        } else {
            return FALSE;
        }
    }
    return false;
}

QStringList DBUpdater::updateNow()
{
    QSqlQuery query(*myDB);
    if(!query.exec("SELECT data_structure_rev FROM dbinfos")) {
        qDebug() << "DB Error: 234 - "  << query.lastError();
    } else {
        query.next();
        qDebug() << "Try to update data structur from: " << query.value(0).toInt() << " to: " << THIS_VERSION_DB_DATA_STRUCTURE;

        if(query.value(0).toInt() == 2 && THIS_VERSION_DB_DATA_STRUCTURE == 3) {

            return update_2_3();
        }
        //TODO later updates
    }
}

bool DBUpdater::update_1_2()
{

    if (myDB->tables().contains("dbinfos")) {
        return true;
    } else {
        QSqlQuery query(*myDB);
        if(!query.exec("CREATE TABLE dbinfos (id INTEGER PRIMARY KEY NOT NULL, data_structure_rev INTEGER NOT NULL)")) {
            qDebug() << "DB Error: 173 - "  << query.lastError();
        } else if(!query.exec("REPLACE INTO dbinfos (id, data_structure_rev) VALUES ( 0,2 )")) {
            qDebug() << "DB Error: 174 - "  << query.lastError();
        } else if(!query.exec("ALTER TABLE activity ADD COLUMN noncontinoustype INTEGER NOT NULL DEFAULT 0")) {
            qDebug() << "DB Error: 175 - "  << query.lastError();
        } else if(!query.exec("ALTER TABLE pupil ADD COLUMN recitalinterval INTEGER NOT NULL DEFAULT 5")) {
            qDebug() << "DB Error: 176 - "  << query.lastError();
        } else if(!query.exec("CREATE TABLE piececomposer (piececomposerid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, composer TEXT)")) {
            qDebug() << "DB Error: 177 - "  << query.lastError();
        } else if(!query.exec("INSERT INTO piececomposer (composer) VALUES ('')")) {
            qDebug() << "DB Error: 185 - "  << query.lastError();
        } else if(!query.exec("ALTER TABLE piece ADD COLUMN piececomposerid INTEGER DEFAULT 1")) {
            qDebug() << "DB Error: 178 - "  << query.lastError();
        } else if(!query.exec("CREATE TABLE recital (recitalid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, desc TEXT, date TEXT, time TEXT, location TEXT, organisator TEXT, defaultaccompanist TEXT, state INTEGER DEFAULT 0)")) {
            qDebug() << "DB Error: 179 - "  << query.lastError();
        } else if(!query.exec("CREATE TABLE recitalarchive (recitalid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, desc TEXT, data TEXT)")) {
            qDebug() << "DB Error: 215 - "  << query.lastError();
        } else if(!query.exec("CREATE TABLE pieceatrecital (parid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, pieceid INTEGER NOT NULL, recitalid INTEGER NOT NULL, sorting INTEGER DEFAULT 0, ifexternalpiece INTEGER DEFAULT 0)")) {
            qDebug() << "DB Error: 180 - "  << query.lastError();
        } else if(!query.exec("CREATE TABLE externalrecitalpiece (erpid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, composer TEXT, title TEXT, genre TEXT, duration INTEGER, musician TEXT)")) {
            qDebug() << "DB Error: 224 - "  << query.lastError();
        } else {
            return true;
        }
    }
    return false;
}

QStringList DBUpdater::update_2_3()
{
    QStringList updateReturn;
    updateReturn.insert(1, "2");
    updateReturn.insert(2, "3");

    QSqlQuery query(*myDB);
    if(!query.exec("CREATE TABLE reminder (reminderid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, desc TEXT, mode INTEGER DEFAULT 0, pupilid INTEGER, notificationsound INTEGER DEFAULT 0)")) {
        qDebug() << "DB Error: 235 - "  << query.lastError();
        updateReturn.insert(3, "235 - "+query.lastError().text());
    } else if(!query.exec("ALTER TABLE activity ADD COLUMN continoustype INTEGER NOT NULL DEFAULT 0")) {
        qDebug() << "DB Error: 253 - "  << query.lastError();
    } else if(!query.exec("UPDATE activity SET continousstopdate = '9999-99-99' WHERE continousstopdate IS NULL")) {
        qDebug() << "DB Error: 256 - "  << query.lastError();
    } else if(!query.exec("ALTER TABLE pupil ADD COLUMN ensembleactivityrequested INTEGER NOT NULL DEFAULT 1")) {
        qDebug() << "DB Error: 261 - "  << query.lastError();
    } else if(!query.exec("REPLACE INTO dbinfos (id, data_structure_rev) VALUES ( 0,3 )")) {
        qDebug() << "DB Error: 236 - "  << query.lastError();
        updateReturn.insert(3, "236 - "+query.lastError().text());
    } else {
        updateReturn.insert(0, "1");
        return updateReturn;
    }
    updateReturn.insert(0, "0");
    return updateReturn;
}

