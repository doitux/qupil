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
#include "mydbhandler.h"

#include <iostream>

#include <QtCore>
#include <QtSql>

#include "mainwindowimpl.h"
#include "configfile.h"
#include "obsoletepersonaldataconverter.h"
#include "dbupdater.h"

using namespace std;

myDBHandler::myDBHandler(ConfigFile *c)
    : myConfig(c)
{
    myPupilDB = new QSqlDatabase;
    *myPupilDB = QSqlDatabase::addDatabase("QSQLITE");

    myDBUpdater = new DBUpdater(myConfig, myPupilDB);

    initDB();
}

myDBHandler::~myDBHandler()
{
}


bool myDBHandler::initDB()
{

    QString pupilDBPath(QString::fromUtf8(myConfig->readConfigString("UserDir").c_str())+"/db/qupil.db");
    QFile pupilDbFile(pupilDBPath);
    bool ok;

    if (!pupilDbFile.exists()) {

        myPupilDB->setDatabaseName(pupilDBPath);
        ok = myPupilDB->open();

        if(ok) {
            QSqlQuery initPupilDbQuery(*myPupilDB);

            initPupilDbQuery.exec("CREATE TABLE pupil (pupilid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, surname TEXT, forename TEXT, address TEXT, email TEXT, telefon TEXT, handy TEXT, birthday TEXT, notes TEXT, fathername TEXT, fatherjob TEXT, fathertelefon TEXT, mothername TEXT, motherjob TEXT, mothertelefon TEXT, firstlessondate TEXT, instrumenttype TEXT, instrumentsize TEXT, ifinstrumentnextsize INTEGER, ifrentinstrument INTEGER, rentinstrumentdesc TEXT, rentinstrumentstartdate TEXT)");
            if (initPupilDbQuery.lastError().isValid()) qDebug() << "DB Error: 1 - "  << initPupilDbQuery.lastError();

            initPupilDbQuery.exec("CREATE TABLE pupilarchive (pupilid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, surname TEXT, forename TEXT, data TEXT)");
            if (initPupilDbQuery.lastError().isValid()) qDebug() << "DB Error: 2 - "  << initPupilDbQuery.lastError();

            // 		state: 1==active, 0==inactive; type: 1==singlelesson, 2==grouplesson, 3==ensemblelesson; unsteadylesson: 1==unsteady, 0==steady
            initPupilDbQuery.exec("CREATE TABLE lesson (lessonid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, state INTEGER, type INTEGER, autolessonname INTEGER, lessonname TEXT, unsteadylesson INTEGER, lessonday INTEGER, lessonstarttime TEXT, lessonstoptime TEXT, lessonlocation TEXT)");
            if (initPupilDbQuery.lastError().isValid()) qDebug() << "DB Error: 3 - "  << initPupilDbQuery.lastError();

            initPupilDbQuery.exec("CREATE TABLE lastlessonname ( llnid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, lessonname TEXT)");
            if (initPupilDbQuery.lastError().isValid()) qDebug() << "DB Error: 4 - "  << initPupilDbQuery.lastError();

            initPupilDbQuery.exec("CREATE TABLE pupilatlesson (palid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, lessonid INTEGER NOT NULL, pupilid INTEGER NOT NULL, llnid INTEGER, startdate TEXT, stopdate TEXT)");
            if (initPupilDbQuery.lastError().isValid()) qDebug() << "DB Error: 5 - "  << initPupilDbQuery.lastError();
            // 		state: 0 == "Geplant", 1 == "In Arbeit", 2 == "Pause", 3 == "Vorspielreif", 4 == "Abgeschlossen"
            initPupilDbQuery.exec("CREATE TABLE piece (pieceid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, cpieceid INTEGER, palid INTEGER, title TEXT, genre Text, duration INTEGER, startdate TEXT, stopdate TEXT, state INTEGER)");
            if (initPupilDbQuery.lastError().isValid()) qDebug() << "DB Error: 6 - "  << initPupilDbQuery.lastError();

            initPupilDbQuery.exec("CREATE TABLE note (noteid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, cnoteid INTEGER, palid INTEGER, date TEXT, content TEXT)");
            if (initPupilDbQuery.lastError().isValid()) qDebug() << "DB Error: 7 - "  << initPupilDbQuery.lastError();

            initPupilDbQuery.exec("CREATE TABLE activity (activityid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, pupilid INTEGER, ifcontinous INTEGER, desc TEXT, continousday INTEGER, continoustime TEXT, date TEXT, continousstopdate TEXT)");
            if (initPupilDbQuery.lastError().isValid()) qDebug() << "DB Error: 8 - "  << initPupilDbQuery.lastError();

            initPupilDbQuery.exec("CREATE TABLE sheetmusiclibrary (smlid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, author INTEGER, title TEXT, publisher INTEGER, rentpupilid INTEGER, lastrentdate TEXT)");
            if (initPupilDbQuery.lastError().isValid()) qDebug() << "DB Error: 9 - "  << initPupilDbQuery.lastError();

            initPupilDbQuery.exec("CREATE TABLE smlauthor (smlauthorid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, author TEXT)");
            if (initPupilDbQuery.lastError().isValid()) qDebug() << "DB Error: 10 - "  << initPupilDbQuery.lastError();

            initPupilDbQuery.exec("CREATE TABLE smlpublisher (smlpublisherid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, publisher TEXT)");
            if (initPupilDbQuery.lastError().isValid()) qDebug() << "DB Error: 11 - "  << initPupilDbQuery.lastError();

        }
    } else {
        //check if there is a backup to replay
        QFile backupFile(pupilDBPath+".new");

        if(backupFile.exists()) {
            QFile oldFile(pupilDBPath+".old");
            if(oldFile.exists()) {
                oldFile.remove();
            }
            pupilDbFile.rename(pupilDBPath+".old");
            backupFile.rename(pupilDBPath);
        }

        myPupilDB->setDatabaseName(pupilDBPath);
        ok = myPupilDB->open();

    }
    return ok;
}

bool myDBHandler::convertObsoletePersonalData()
{

    myW->updateConvertOldDataProgress(1, QString("Konvertiere alte Daten ..."));

    QDir oldContentDir(QString::fromUtf8(myConfig->readConfigString("UserDir").c_str())+"/content");

    QStringList subdirsTemp;
    subdirsTemp.clear();
    subdirsTemp = oldContentDir.entryList();
    subdirsTemp.removeAt(subdirsTemp.indexOf("."));
    subdirsTemp.removeAt(subdirsTemp.indexOf(".."));
    subdirsTemp.removeAt(subdirsTemp.indexOf("groups"));

    QDir subDir;
    QFile personaldataFile;
    QFile groupReportsFile;
    QFile groupPiecesFile;
    QString line;
    QStringList lines;
    QString forename;
    QString surname;
    QString partofname;

    QSqlQuery insertLessonQuery;

    int pupilIdCounter = 0;
    int lessonIdCounter = 0;

    if (!subdirsTemp.empty()) {

        for ( QStringList::Iterator it = subdirsTemp.begin(); it != subdirsTemp.end(); ++it ) {

            // 			qDebug() << *it;
            pupilIdCounter++;

            myW->updateConvertOldDataProgress(pupilIdCounter, QString("Konvertiere Daten von: "+*it+" ..."));

            //convert old personal data
            subDir.setPath(oldContentDir.absolutePath()+"/"+*it);
            personaldataFile.setFileName(subDir.absolutePath()+"/personaldata.txt");

            partofname = *it;
            forename = partofname.section( "_", 1, 1 );
            surname = partofname.section( "_", 0, 0 );

            PersonalData myData(surname+", "+forename);

            QString singlelesson1day;
            QString singlelesson2day;
            QString singlelesson3day;
            if (myData.readData("singlelesson1day","") == "Montag")
                singlelesson1day = "0";
            if (myData.readData("singlelesson2day","") == "Montag")
                singlelesson2day = "0";
            if (myData.readData("singlelesson3day","") == "Montag")
                singlelesson3day = "0";
            if (myData.readData("singlelesson1day","") == "Dienstag")
                singlelesson1day = "1";
            if (myData.readData("singlelesson2day","") == "Dienstag")
                singlelesson2day = "1";
            if (myData.readData("singlelesson3day","") == "Dienstag")
                singlelesson3day = "1";
            if (myData.readData("singlelesson1day","") == "Mittwoch")
                singlelesson1day = "2";
            if (myData.readData("singlelesson2day","") == "Mittwoch")
                singlelesson2day = "2";
            if (myData.readData("singlelesson3day","") == "Mittwoch")
                singlelesson3day = "2";
            if (myData.readData("singlelesson1day","") == "Donnerstag")
                singlelesson1day = "3";
            if (myData.readData("singlelesson2day","") == "Donnerstag")
                singlelesson2day = "3";
            if (myData.readData("singlelesson3day","") == "Donnerstag")
                singlelesson3day = "3";
            if (myData.readData("singlelesson1day","") == "Freitag")
                singlelesson1day = "4";
            if (myData.readData("singlelesson2day","") == "Freitag")
                singlelesson2day = "4";
            if (myData.readData("singlelesson3day","") == "Freitag")
                singlelesson3day = "4";
            if (myData.readData("singlelesson1day","") == "Samstag")
                singlelesson1day = "5";
            if (myData.readData("singlelesson2day","") == "Samstag")
                singlelesson2day = "5";
            if (myData.readData("singlelesson3day","") == "Samstag")
                singlelesson3day = "5";
            if (myData.readData("singlelesson1day","") == "Sonntag")
                singlelesson1day = "6";
            if (myData.readData("singlelesson2day","") == "Sonntag")
                singlelesson2day = "6";
            if (myData.readData("singlelesson3day","") == "Sonntag")
                singlelesson3day = "6";

            if(singlelesson1day.isNull()) singlelesson1day = "0";
            if(singlelesson2day.isNull()) singlelesson2day = "0";
            if(singlelesson3day.isNull()) singlelesson3day = "0";

            QSqlQuery myConverterQuery("INSERT INTO pupil ( pupilid, forename, surname, address, email, telefon, handy, birthday, notes, fathername, fatherjob, fathertelefon, mothername, motherjob, mothertelefon, firstlessondate, instrumenttype, instrumentsize, ifinstrumentnextsize, ifrentinstrument, rentinstrumentdesc, rentinstrumentstartdate) VALUES ( "+QString("%1").arg(pupilIdCounter,10)+", '"+forename+"', '"+surname+"', '"+myData.readData("address","")+"', '"+myData.readData("email","")+"', '"+myData.readData("telefon","")+"', '"+myData.readData("handy","")+"', '"+myData.readData("birthday","")+"', '"+myData.readData("notes","")+"', '"+myData.readData("fathername","")+"', '"+myData.readData("fatherjob","")+"', '"+myData.readData("fathertelefon","")+"', '"+myData.readData("mothername","")+"', '"+myData.readData("motherjob","")+"', '"+myData.readData("mothertelefon","")+"', '"+myData.readData("firstlesson","")+"', '"+myData.readData("instrumenttype","")+"', '"+myData.readData("instrumentsize","")+"', "+myData.readData("instrumentnextsize","0")+", "+myData.readData("rentinstrument","")+", '"+myData.readData("instrumentdesc","")+"', '"+myData.readData("instrumentrentdate","")+"')", *myPupilDB );
            if (myConverterQuery.lastError().isValid()) qDebug() << "DB Error: 12 - "  << myConverterQuery.lastError();

            if(myData.readData("ifsinglelesson1","") == "1") {

                lessonIdCounter++;
                QString currentPalIdString;
                QString lessonDuration =  QString::number(QTime::fromString(myData.readData("singlelesson1starttime",""),"hh:mm").secsTo(QTime::fromString(myData.readData("singlelesson1stoptime",""),"hh:mm"))/60,10);

                insertLessonQuery.exec("INSERT INTO lesson (lessonid, state, type, autolessonname, lessonname, unsteadylesson, lessonday, lessonstarttime, lessonstoptime) VALUES ( "+QString("%1").arg(lessonIdCounter,10)+", 1, 1, 1, 'EU - "+lessonDuration+" Min. - "+forename.mid(0,3)+surname.mid(0,3)+"', 0, "+singlelesson1day+", '"+myData.readData("singlelesson1starttime","")+"', '"+myData.readData("singlelesson1stoptime", "")+"')");
                if (insertLessonQuery.lastError().isValid()) qDebug() << "DB Error: 13 - " << insertLessonQuery.lastError();

                insertLessonQuery.exec("INSERT INTO pupilatlesson (lessonid, pupilid, startdate, stopdate) VALUES ( "+QString("%1").arg(lessonIdCounter,10)+", "+QString("%1").arg(pupilIdCounter,10)+", '"+getFirstReportDate(*it)+"', '9999-99-99')");
                if (insertLessonQuery.lastError().isValid()) qDebug() << "DB Error: 14 - " << insertLessonQuery.lastError();
                QSqlQuery queryPalId("SELECT last_insert_rowid()", *myPupilDB );
                if (queryPalId.lastError().isValid()) qDebug() << "DB Error: 15 - " << queryPalId.lastError();
                queryPalId.next();
                currentPalIdString = queryPalId.value(0).toString();

                convertOldSingleLessonContents(*it, currentPalIdString);
            }
            if(myData.readData("ifsinglelesson2","") == "1") {

                lessonIdCounter++;
                QString currentPalIdString;
                QString lessonDuration =  QString::number(QTime::fromString(myData.readData("singlelesson1starttime",""),"hh:mm").secsTo(QTime::fromString(myData.readData("singlelesson1stoptime",""),"hh:mm"))/60,10);
                insertLessonQuery.exec("INSERT INTO lesson (lessonid, state, type, autolessonname, lessonname, unsteadylesson, lessonday, lessonstarttime, lessonstoptime) VALUES ( "+QString("%1").arg(lessonIdCounter,10)+", 1, 1, 1, 'EU - "+lessonDuration+" Min. - "+forename.mid(0,3)+surname.mid(0,3)+"', 0, "+singlelesson2day+", '"+myData.readData("singlelesson2starttime","")+"', '"+myData.readData("singlelesson2stoptime","")+"')");
                if (insertLessonQuery.lastError().isValid()) qDebug() << "DB Error: 16 - " << insertLessonQuery.lastError();

                insertLessonQuery.exec("INSERT INTO pupilatlesson (lessonid, pupilid, startdate, stopdate) VALUES ( "+QString("%1").arg(lessonIdCounter,10)+", "+QString("%1").arg(pupilIdCounter,10)+", '"+getFirstReportDate(*it)+"', '9999-99-99')");
                if (insertLessonQuery.lastError().isValid()) qDebug() << "DB Error: 17 - " << insertLessonQuery.lastError();
                QSqlQuery queryPalId("SELECT last_insert_rowid()", *myPupilDB );
                if (queryPalId.lastError().isValid()) qDebug() << "DB Error: 18 - " << queryPalId.lastError();
                queryPalId.next();
                currentPalIdString = queryPalId.value(0).toString();

                convertOldSingleLessonContents(*it, currentPalIdString);
            }
            if(myData.readData("ifsinglelesson3","") == "1") {

                lessonIdCounter++;
                QString currentPalIdString;
                QString lessonDuration =  QString::number(QTime::fromString(myData.readData("singlelesson1starttime",""),"hh:mm").secsTo(QTime::fromString(myData.readData("singlelesson1stoptime",""),"hh:mm"))/60,10);
                insertLessonQuery.exec("INSERT INTO lesson (lessonid, state, type, autolessonname, lessonname, unsteadylesson, lessonday, lessonstarttime, lessonstoptime) VALUES ( "+QString("%1").arg(lessonIdCounter,10)+", 1, 1, 1, 'EU - "+lessonDuration+" Min. - "+forename.mid(0,3)+surname.mid(0,3)+"', 0, "+singlelesson3day+", '"+myData.readData("singlelesson3starttime","")+"', '"+myData.readData("singlelesson3stoptime","")+"')");
                if (insertLessonQuery.lastError().isValid()) qDebug() << "DB Error: 19 - " << insertLessonQuery.lastError();

                insertLessonQuery.exec("INSERT INTO pupilatlesson (lessonid, pupilid, startdate, stopdate) VALUES ( "+QString("%1").arg(lessonIdCounter,10)+", "+QString("%1").arg(pupilIdCounter,10)+", '"+getFirstReportDate(*it)+"', '9999-99-99')");
                if (insertLessonQuery.lastError().isValid()) qDebug() << "DB Error: 20 - " << insertLessonQuery.lastError();
                QSqlQuery queryPalId("SELECT last_insert_rowid()", *myPupilDB );
                if (queryPalId.lastError().isValid()) qDebug() << "DB Error: 21 - " << queryPalId.lastError();
                queryPalId.next();
                currentPalIdString = queryPalId.value(0).toString();

                convertOldSingleLessonContents(*it, currentPalIdString);
            }

            if(myData.readData("ifvarlessontime","") == "1") {

                lessonIdCounter++;
                QString currentPalIdString;
                insertLessonQuery.exec("INSERT INTO lesson (lessonid, state, type, autolessonname, lessonname, unsteadylesson) VALUES ( "+QString("%1").arg(lessonIdCounter,10)+", 1, 1, 1, 'EU - "+forename.mid(0,3)+surname.mid(0,3)+"', 1)");
                if (insertLessonQuery.lastError().isValid()) qDebug() << "DB Error: 22 - " << insertLessonQuery.lastError();

                insertLessonQuery.exec("INSERT INTO pupilatlesson (lessonid, pupilid, startdate, stopdate) VALUES ( "+QString("%1").arg(lessonIdCounter,10)+", "+QString("%1").arg(pupilIdCounter,10)+", '"+getFirstReportDate(*it)+"', '9999-99-99')");
                if (insertLessonQuery.lastError().isValid()) qDebug() << "DB Error: 23 - " << insertLessonQuery.lastError();
                QSqlQuery queryPalId("SELECT last_insert_rowid()", *myPupilDB );
                if (queryPalId.lastError().isValid()) qDebug() << "DB Error: 24 - " << queryPalId.lastError();
                queryPalId.next();
                currentPalIdString = queryPalId.value(0).toString();

                convertOldSingleLessonContents(*it, currentPalIdString);
            }

        }
    }

    // 	//search for group and enseblelesson
    QDir oldGroupDir(QString::fromUtf8(myConfig->readConfigString("UserDir").c_str())+"/content/groups/");
    QFile groupsFile (oldGroupDir.absolutePath()+"/groups.txt");

    int groupCounter = 0;

    if ( groupsFile.exists() ) {

        QString line;
        QString groupPupil;
        QString groupPupilSurname;
        QString groupPupilForename;
        QString groupName;
        QString dayId;

        if (groupsFile.open( QIODevice::ReadOnly )) {
            QTextStream readStream( &groupsFile );
            while ( !readStream.atEnd() ) {
                line = readStream.readLine();

                groupName = line.section( "=", 0, 0 );

                if (line.section( "=", 1, 1 ) == "Montag") dayId = "0";
                if (line.section( "=", 1, 1 ) == "Dienstag") dayId = "1";
                if (line.section( "=", 1, 1 ) == "Mittwoch") dayId = "2";
                if (line.section( "=", 1, 1 ) == "Donnerstag") dayId = "3";
                if (line.section( "=", 1, 1 ) == "Freitag") dayId = "4";
                if (line.section( "=", 1, 1 ) == "Samstag") dayId = "5";
                if (line.section( "=", 1, 1 ) == "Sonntag") dayId = "6";

                //Convert groups details

                lessonIdCounter++;
                groupCounter++;
                QStringList palIdStringList;
                myW->updateConvertOldDataProgress(pupilIdCounter+groupCounter, QString("Konvertiere Daten von: "+groupName+" ..."));

                QSqlQuery myGroupsQuery("INSERT INTO lesson (lessonid, state, type, autolessonname, lessonname, unsteadylesson, lessonday, lessonstarttime, lessonstoptime) VALUES ( "+QString("%1").arg(lessonIdCounter,10)+", 1, 2, 0, '"+groupName+"', 0, "+dayId+", '"+line.section( "=", 2, 2 )+"', '"+line.section( "=", 3, 3 )+"')", *myPupilDB);
                if (myGroupsQuery.lastError().isValid()) qDebug() << "DB Error: 25 - " << myGroupsQuery.lastError();

                if ( line.count("=",Qt::CaseSensitive) > 4) {
                    int i;
                    for (i=4; i<line.count("=",Qt::CaseSensitive); i++) {
                        // 						qDebug() << i << line.count("=",Qt::CaseSensitive);
                        groupPupil = line.section( "=", i, i );
                        groupPupilSurname = groupPupil.section( ", ", 0, 0 );
                        groupPupilForename = groupPupil.section( ", ", 1, 1 );

                        QString pupilId;

                        QSqlQuery tempQuery("SELECT pupilid FROM pupil WHERE forename='"+groupPupilForename+"' AND surname='"+groupPupilSurname+"'", *myPupilDB);
                        while (tempQuery.next()) {
                            pupilId = tempQuery.value(0).toString();
                        }
                        if (tempQuery.lastError().isValid()) qDebug() << "DB Error: 26 - " << tempQuery.lastError();

                        //getFirstReportDate for PupilAtGroup startdate
                        QDate date (1900,01,01);
                        groupReportsFile.setFileName(oldGroupDir.absolutePath()+"/"+groupName+"_report.txt");
                        if(groupReportsFile.exists()) {
                            if ( groupReportsFile.open( QIODevice::ReadOnly ) ) {
                                QString line;
                                QTextStream stream( &groupReportsFile );
                                line = stream.readLine();
                                if(!line.isNull()) {
                                    date = QDate::fromString(line.section("\t",0,0), "dd.MM.yyyy");
                                }
                                groupReportsFile.close();
                            }
                        }

                        QSqlQuery myPupilAtGroupsQuery("INSERT INTO pupilatlesson (lessonid, pupilid, startdate, stopdate) VALUES ("+QString("%1").arg(lessonIdCounter,10)+", "+pupilId+", '"+date.toString(Qt::ISODate)+"', '9999-99-99')", *myPupilDB);
                        if (myPupilAtGroupsQuery.lastError().isValid()) qDebug() << "DB Error: 27 - " << myPupilAtGroupsQuery.lastError();

                        QSqlQuery queryPalId("SELECT last_insert_rowid()", *myPupilDB );
                        if (queryPalId.lastError().isValid()) qDebug() << "DB Error: 28 - " << queryPalId.lastError();
                        queryPalId.next();
                        // 						put currentPalId in StringList for converting notes and pieces
                        palIdStringList << queryPalId.value(0).toString();
                    }
                }
                // 				going to convert pieces and notes now
                //Convert group reports/pieces
                groupReportsFile.setFileName(oldGroupDir.absolutePath()+"/"+groupName+"_report.txt");
                if(groupReportsFile.exists()) {

                    QStringList gReportLines;
                    QString gReportTemp;
                    QString tempLine;

                    if ( groupReportsFile.open( QIODevice::ReadOnly ) ) {
                        QTextStream stream( &groupReportsFile );
                        while ( !stream.atEnd() ) {
                            tempLine = stream.readLine();
                            gReportLines += tempLine;
                        }
                        groupReportsFile.close();
                    }

                    for ( QStringList::Iterator it = gReportLines.begin(); it != gReportLines.end(); ++it ) {

                        gReportTemp = *it;
                        gReportTemp.remove("\"");
                        gReportTemp.remove("'");
                        QDate date = QDate::fromString(gReportTemp.section("\t",0,0), "dd.MM.yyyy");

                        int commonNoteId = -1;
                        for ( QStringList::Iterator it = palIdStringList.begin(); it != palIdStringList.end(); ++it ) {

                            QSqlQuery myGroupReportsQuery("INSERT INTO note (palid, date, content) VALUES ( "+*it+", '"+date.toString(Qt::ISODate)+"', '"+gReportTemp.section("\t",1,1)+"')", *myPupilDB);
                            if (myGroupReportsQuery.lastError().isValid()) qDebug() << "DB Error: 29 - " << "groupreport" << *it << myGroupReportsQuery.lastError();

                            QSqlQuery myCommonNoteQuery(*myPupilDB);
                            myCommonNoteQuery.prepare("UPDATE note SET cnoteid = ? WHERE noteid = ?");
                            // 						just init commonNoteId for the first time
                            if(commonNoteId == -1) commonNoteId = myGroupReportsQuery.lastInsertId().toInt();
                            myCommonNoteQuery.addBindValue(commonNoteId);
                            myCommonNoteQuery.addBindValue(myGroupReportsQuery.lastInsertId().toInt());
                            myCommonNoteQuery.exec();
                            if(myCommonNoteQuery.lastError().isValid()) qDebug() << "DB Error: 30 - " << myCommonNoteQuery.lastError();
                        }
                    }
                }

                //convert old pieces
                groupPiecesFile.setFileName(oldGroupDir.absolutePath()+"/"+groupName+"_pieces.txt");

                if(groupPiecesFile.exists()) {

                    QStringList gPiecesLines;
                    QString gPiecesTemp;
                    QString tempLine;

                    if ( groupPiecesFile.open( QIODevice::ReadOnly ) ) {
                        QTextStream stream( &groupPiecesFile );
                        while ( !stream.atEnd() ) {
                            tempLine = stream.readLine();
                            gPiecesLines += tempLine;
                        }
                        groupPiecesFile.close();
                    }

                    for ( QStringList::Iterator it = gPiecesLines.begin(); it != gPiecesLines.end(); ++it ) {

                        gPiecesTemp = *it;
                        gPiecesTemp.remove("\"");
                        gPiecesTemp.remove("'");

                        QString stateIntString;
                        if(gPiecesTemp.section("#",3,3) == "Geplant") stateIntString = "0";
                        if(gPiecesTemp.section("#",3,3) == "In Arbeit") stateIntString = "1";
                        if(gPiecesTemp.section("#",3,3) == "Pause") stateIntString = "2";
                        if(gPiecesTemp.section("#",3,3) == "Vorspielreif") stateIntString = "3";
                        if(gPiecesTemp.section("#",3,3) == "Abgeschlossen") stateIntString = "4";


                        QDate startDate = QDate::fromString(gPiecesTemp.section("#",2,2), "dd.MM.yyyy");
                        QDate stopDate = QDate::fromString(gPiecesTemp.section("#",4,4), "dd.MM.yyyy");

                        int commonPieceId = -1;
                        for ( QStringList::Iterator it = palIdStringList.begin(); it != palIdStringList.end(); ++it ) {

                            QSqlQuery myGroupPiecesQuery("INSERT INTO piece (palid, title, genre, startdate, stopdate, state) VALUES ( "+*it+", '"+gPiecesTemp.section("#",0,0)+"', '"+gPiecesTemp.section("#",1,1)+"', '"+startDate.toString(Qt::ISODate)+"', '"+stopDate.toString(Qt::ISODate)+"', "+stateIntString+")", *myPupilDB);
                            if (myGroupPiecesQuery.lastError().isValid()) qDebug() << "DB Error: 31 - " << "grouppiece" << *it << myGroupPiecesQuery.lastError();
                            QSqlQuery myCommonPieceQuery(*myPupilDB);
                            myCommonPieceQuery.prepare("UPDATE piece SET cpieceid = ? WHERE pieceid = ?");
                            // 						just init commonNoteId for the first time
                            if(commonPieceId == -1) commonPieceId = myGroupPiecesQuery.lastInsertId().toInt();
                            myCommonPieceQuery.addBindValue(commonPieceId);
                            myCommonPieceQuery.addBindValue(myGroupPiecesQuery.lastInsertId().toInt());
                            myCommonPieceQuery.exec();
                            if(myCommonPieceQuery.lastError().isValid()) qDebug() << "DB Error: 32 - " << myCommonPieceQuery.lastError();
                        }
                    }
                }
            }
            groupsFile.close();
        }
    }

    myW->updateConvertOldDataProgress(-1, QString("Konvertieren der alten Daten beendet!"));

    return 1;
}

void myDBHandler::convertOldSingleLessonContents(QString pupilPath, QString currentPalIdString)
{

    // 	convert old reports
    //
    QString line;

    QDir oldContentDir(QString::fromUtf8(myConfig->readConfigString("UserDir").c_str())+"/content");
    QDir subDir(oldContentDir.absolutePath()+"/"+pupilPath);
    QFile reportsFile;
    QFile piecesFile;

    reportsFile.setFileName(subDir.absolutePath()+"/report.txt");

    if(reportsFile.exists()) {

        QStringList reportLines;
        QString reportTemp;

        if ( reportsFile.open( QIODevice::ReadOnly ) ) {
            QTextStream stream( &reportsFile );
            while ( !stream.atEnd() ) {
                line = stream.readLine();
                reportLines += line;
            }
            reportsFile.close();
        }

        for ( QStringList::Iterator it = reportLines.begin(); it != reportLines.end(); ++it ) {

            reportTemp = *it;
            reportTemp.remove("\"");
            reportTemp.remove("'");
            QDate date = QDate::fromString(reportTemp.section("\t",0,0), "dd.MM.yyyy");

            QSqlQuery myReportsQuery("INSERT INTO note (palid, date, content) VALUES ( "+currentPalIdString+", '"+date.toString(Qt::ISODate)+"', '"+reportTemp.section("\t",1,1)+"')", *myPupilDB);
            if (myReportsQuery.lastError().isValid()) qDebug() << "DB Error: 33 - " << myReportsQuery.lastError();
            QSqlQuery myCommonNoteQuery(*myPupilDB);
            myCommonNoteQuery.prepare("UPDATE note SET cnoteid = ? WHERE noteid = ?");
            int lastId = myReportsQuery.lastInsertId().toInt();
            myCommonNoteQuery.addBindValue(lastId);
            myCommonNoteQuery.addBindValue(lastId);
            myCommonNoteQuery.exec();
            if(myCommonNoteQuery.lastError().isValid()) qDebug() << "DB Error: 34 - " << myCommonNoteQuery.lastError();
        }
    }

    //convert old pieces
    subDir.setPath(oldContentDir.absolutePath()+"/"+pupilPath);
    piecesFile.setFileName(subDir.absolutePath()+"/pieces.txt");


    if(piecesFile.exists()) {

        QStringList piecesLines;
        QString piecesTemp;

        if ( piecesFile.open( QIODevice::ReadOnly ) ) {
            QTextStream stream( &piecesFile );
            while ( !stream.atEnd() ) {
                line = stream.readLine();
                piecesLines += line;
            }
            piecesFile.close();
        }

        for ( QStringList::Iterator it = piecesLines.begin(); it != piecesLines.end(); ++it ) {

            piecesTemp = *it;
            piecesTemp.remove("\"");
            piecesTemp.remove("'");

            QString stateIntString;
            if(piecesTemp.section("#",3,3) == "Geplant") stateIntString = "0";
            if(piecesTemp.section("#",3,3) == "In Arbeit") stateIntString = "1";
            if(piecesTemp.section("#",3,3) == "Pause") stateIntString = "2";
            if(piecesTemp.section("#",3,3) == "Vorspielreif") stateIntString = "3";
            if(piecesTemp.section("#",3,3) == "Abgeschlossen") stateIntString = "4";

            QDate startDate = QDate::fromString(piecesTemp.section("#",2,2), "dd.MM.yyyy");
            QDate stopDate = QDate::fromString(piecesTemp.section("#",4,4), "dd.MM.yyyy");

            QSqlQuery myPiecesQuery("INSERT INTO piece (palid, title, genre, startdate, stopdate, state) VALUES ( "+currentPalIdString+", '"+piecesTemp.section("#",0,0)+"', '"+piecesTemp.section("#",1,1)+"', '"+startDate.toString(Qt::ISODate)+"', '"+stopDate.toString(Qt::ISODate)+"', "+stateIntString+")", *myPupilDB);
            if (myPiecesQuery.lastError().isValid()) qDebug() << "DB Error: 35 - " << myPiecesQuery.lastError();
            QSqlQuery myCommonPieceQuery(*myPupilDB);
            myCommonPieceQuery.prepare("UPDATE piece SET cpieceid = ? WHERE pieceid = ?");
            int lastId = myPiecesQuery.lastInsertId().toInt();
            myCommonPieceQuery.addBindValue(lastId);
            myCommonPieceQuery.addBindValue(lastId);
            myCommonPieceQuery.exec();
            if(myCommonPieceQuery.lastError().isValid()) qDebug() << "DB Error: 36 - " << myCommonPieceQuery.lastError();
        }
    }
}

QString myDBHandler::getFirstReportDate(QString pupilPath)
{

    QDate date(1900,01,01);
    QString line;

    QDir oldContentDir(QString::fromUtf8(myConfig->readConfigString("UserDir").c_str())+"/content");
    QDir subDir(oldContentDir.absolutePath()+"/"+pupilPath);
    QFile reportsFile;

    reportsFile.setFileName(subDir.absolutePath()+"/report.txt");

    if(reportsFile.exists()) {

        QStringList reportLines;
        QString reportTemp;

        if ( reportsFile.open( QIODevice::ReadOnly ) ) {
            QTextStream stream( &reportsFile );
            line = stream.readLine();
            if(!line.isNull()) {
                date = QDate::fromString(line.section("\t",0,0), "dd.MM.yyyy");
            }
            reportsFile.close();
        }
    }
    QString returnString;
    returnString = date.toString(Qt::ISODate);
    return returnString;
}

bool myDBHandler::closeDB()
{
    qDebug() << myPupilDB->commit();
    qDebug() << myPupilDB->rollback();
    myPupilDB->close();
    myPupilDB->close();
    return TRUE;
}

bool myDBHandler::updateDBStructure(int oldVersion, int newVersion)
{

    return myDBUpdater->update(oldVersion, newVersion);
}

bool myDBHandler::dBUpdateNeeded()
{

    return myDBUpdater->checkIfDbUpdateNeeded();
}

QStringList myDBHandler::updateDB()
{

    return myDBUpdater->updateNow();
}

void myDBHandler::removeReminderById(int id)
{

    QSqlQuery query;
    query.prepare("DELETE FROM reminder WHERE reminderid = ?");
    query.addBindValue(id);
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 264 - " << query.lastError();
    }
}

QString myDBHandler::getCurrentLessonStartTime()
{

    QSqlQuery query2;
    query2.prepare("SELECT lessonstarttime FROM lesson WHERE state = 1 AND lessonday = ? AND lessonstarttime <= ? AND lessonstoptime > ?");
    query2.addBindValue(QString::number(QDate::currentDate().dayOfWeek()-1));
    query2.addBindValue(QTime::currentTime().toString("hh:mm"));
    query2.addBindValue(QTime::currentTime().toString("hh:mm"));
    query2.exec();
    if (query2.lastError().isValid()) qDebug() << "DB Error: 265 - " << query2.lastError();
    else {
        if(query2.next()) {

            return query2.value(0).toString();
        }
    }
    return QString("");
}
