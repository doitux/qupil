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
#ifndef MYDBHANDLER_H
#define MYDBHANDLER_H

class ConfigFile;
class mainWindowImpl;
class DBUpdater;
class QSqlDatabase;
class QString;
class QStringList;

/**
 @author Felix Hammer <f.hammer@gmx.de>
*/
class myDBHandler
{
public:
    myDBHandler(ConfigFile *c);

    ~myDBHandler();

    bool initDB();
    bool convertObsoletePersonalData();
    void convertOldSingleLessonContents(QString, QString);
    bool closeDB();
    bool updateDBStructure(int, int);
    bool dBUpdateNeeded();
    QStringList updateDB();

    QString getFirstReportDate(QString);
    void setMyW ( mainWindowImpl* theValue ) {
        myW = theValue;
    }
    QSqlDatabase* getMyPupilDb() const {
        return myPupilDB;
    }

    void removeReminderById(int id);
    QString getCurrentLessonStartTime();

private:

    ConfigFile *myConfig;
    mainWindowImpl *myW;
    DBUpdater *myDBUpdater;
    QSqlDatabase *myPupilDB;

};

#endif
