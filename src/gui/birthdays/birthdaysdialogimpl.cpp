/***************************************************************************
 *   Copyright (C) 2006 by FThauer FHammer   *
 *   f.thauer@web.de   *
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
#include "birthdaysdialogimpl.h"

#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "configfile.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"

BirthdaysDialogImpl::BirthdaysDialogImpl(ConfigFile *c, mainWindowImpl *w)
    : myConfig(c), myW(w)
{
    setupUi(this);
}

int BirthdaysDialogImpl::exec(bool force)
{
    retranslateUi(this);
    checkBirthdays();

    if( lastWeekBirthdays || todayBirthdays)
        return QDialog::exec();
    else if(force)
        return QDialog::exec();
    else
        return 0;
}

BirthdaysDialogImpl::~BirthdaysDialogImpl() {}

void BirthdaysDialogImpl::checkBirthdays()
{

    QDate today(QDate::currentDate());
    QDate lastWeek(today.addDays(-7));

    QFont font = QApplication::font();

    QSqlQuery queryTodayCount("SELECT count(*) FROM pupil WHERE strftime(\"%m-%d\",  birthday) = '"+today.toString("MM-dd")+"'", *myW->getMyDb()->getMyPupilDb());
    if (queryTodayCount.lastError().isValid()) qDebug() << "DB Error: 37 - " << queryTodayCount.lastError();
    queryTodayCount.next();
    todayBirthdays = queryTodayCount.value(0).toInt();

    QSqlQuery queryLastWeekCount("SELECT count(*) FROM pupil WHERE strftime(\"%m-%d\",  birthday) < '"+today.toString("MM-dd")+"' AND strftime(\"%m-%d\",  birthday) > '"+lastWeek.toString("MM-dd")+"' ", *myW->getMyDb()->getMyPupilDb());
    if (queryLastWeekCount.lastError().isValid()) qDebug() << "DB Error: 38 - " << queryLastWeekCount.lastError();
    queryLastWeekCount.next();
    lastWeekBirthdays = queryLastWeekCount.value(0).toInt();

    QString msg("");
    QString todayHead("");
    if(todayBirthdays == 1)
        todayHead = tr("The following student has his birthday today:");
    if(todayBirthdays > 1)
        todayHead = tr("The following students have their birthday today:");
    if(todayBirthdays > 0) {
        msg += "<span style='font-size:"+QString::number(font.pointSize()+3)+"pt; font-weight:600;'>"+QString::fromUtf8(todayHead.toStdString().c_str())+"</span><br><br>";

        QSqlQuery queryToday("SELECT birthday, forename, surname FROM pupil WHERE strftime(\"%m-%d\",  birthday) = '"+today.toString("MM-dd")+"' ORDER BY birthday DESC", *myW->getMyDb()->getMyPupilDb());
        if (queryToday.lastError().isValid()) qDebug() << "DB Error: 39 - " << queryToday.lastError();
        while (queryToday.next()) {
            QDate birthdayThisYear;
            birthdayThisYear.setDate(QDate::currentDate().year(), QDate::fromString(queryToday.value(0).toString(), Qt::ISODate).month(), QDate::fromString(queryToday.value(0).toString(), Qt::ISODate).day());
// 			calculate age
            int daysFromBirthday = today.daysTo(QDate::fromString(queryToday.value(0).toString(), Qt::ISODate));
            int years = daysFromBirthday/365;
            msg += "<span style='font-size:"+QString::number(font.pointSize()+2)+"pt; '>&nbsp;&nbsp;&nbsp;&nbsp;"+birthdayThisYear.toString(Qt::SystemLocaleLongDate)+": </span><span style='font-size:"+QString::number(font.pointSize()+2)+"pt; color: blue; '>"+queryToday.value(1).toString()+" "+queryToday.value(2).toString()+"</span><span style='font-size:"+QString::number(font.pointSize()+1)+"pt; '> - </span><span style='font-size:"+QString::number(font.pointSize()+2)+"pt; color: green; '>"+QString( "%1" ).arg( years ).remove("-")+" "+tr("Years")+"</span><br>";
        }
        msg += "<br>";
    }

    QString lastWeekHead("");
    if(lastWeekBirthdays == 1)
        lastWeekHead = tr("The following student had his birthday last week:");
    if(lastWeekBirthdays > 1)
        lastWeekHead = tr("The following students had their birthday last week:");
    if(lastWeekBirthdays > 0) {
        msg += "<span style='font-size:"+QString::number(font.pointSize()+3)+"pt; font-weight:600;'>"+QString::fromUtf8(lastWeekHead.toStdString().c_str())+"</span><br><br>";

        QSqlQuery queryLastWeek("SELECT birthday, forename, surname FROM pupil WHERE strftime(\"%m-%d\",  birthday) < '"+today.toString("MM-dd")+"' AND strftime(\"%m-%d\",  birthday) > '"+lastWeek.toString("MM-dd")+"' ORDER BY birthday DESC", *myW->getMyDb()->getMyPupilDb());
        if (queryLastWeek.lastError().isValid()) qDebug() << "DB Error: 40 - " << queryLastWeek.lastError();
        while (queryLastWeek.next()) {
            QDate birthdayThisYear;
            birthdayThisYear.setDate(QDate::currentDate().year(), QDate::fromString(queryLastWeek.value(0).toString(), Qt::ISODate).month(), QDate::fromString(queryLastWeek.value(0).toString(), Qt::ISODate).day());
            // calculate age
            int daysFromBirthday = today.daysTo(QDate::fromString(queryLastWeek.value(0).toString(), Qt::ISODate));
            int years = daysFromBirthday/365;
            msg += "<span style='font-size:"+QString::number(font.pointSize()+1)+"pt; '>&nbsp;&nbsp;&nbsp;&nbsp;"+birthdayThisYear.toString(Qt::SystemLocaleLongDate)+": </span><span style='font-size:"+QString::number(font.pointSize()+1)+"pt; color: blue; '>"+queryLastWeek.value(1).toString()+" "+queryLastWeek.value(2).toString()+"</span><span style='font-size:"+QString::number(font.pointSize()+1)+"pt; '> - </span><span style='font-size:"+QString::number(font.pointSize()+1)+"pt; color: green; '>"+QString( "%1" ).arg( years ).remove("-")+" "+tr("Years")+"</span><br>";
        }
    }

    if((!lastWeekBirthdays) && (!todayBirthdays)) {
        msg = QString::fromUtf8(tr("In the last week no student had his birthday!").toStdString().c_str());
    }

    label_txt->setText(msg);
}

