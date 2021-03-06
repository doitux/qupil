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
#include "pupillisttreewidget.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"
#include "qupil_defs.h"
#include "configfile.h"
#include <QtSql>

PupilListTreeWidget::PupilListTreeWidget(QWidget *tab)
    : QTreeWidget(tab), headerSectionIndent(10)
{

    pupilPopupMenu = new QMenu();
    delPupil = new QAction(QIcon(":/gfx/list-remove-user.svg"), QString::fromUtf8(tr("Delete Pupil").toStdString().c_str()), pupilPopupMenu);
    pupilPopupMenu->addAction(delPupil);
    archivePupil = new QAction(QIcon(":/gfx/archive-insert.svg"),QString::fromUtf8(tr("Archive Pupil").toStdString().c_str()), pupilPopupMenu);
    pupilPopupMenu->addAction(archivePupil);
    archiveAndDelPupil = new QAction(QString::fromUtf8(tr("Archive and Delete Pupil").toStdString().c_str()), pupilPopupMenu);
    pupilPopupMenu->addAction(archiveAndDelPupil);

    connect( this, SIGNAL (currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT (pupilListSelectionChanged(QTreeWidgetItem*, QTreeWidgetItem*)) );
    connect( this, SIGNAL ( customContextMenuRequested(const QPoint) ), this, SLOT ( callPupilListContextMenu( const QPoint) ) );
    connect( delPupil, SIGNAL ( triggered() ), this, SLOT ( delCurrentPupil() ) );
    connect( archivePupil, SIGNAL ( triggered() ), this, SLOT ( archiveCurrentPupil() ) );
    connect( archiveAndDelPupil, SIGNAL ( triggered() ), this, SLOT ( archiveAndDelCurrentPupil() ) );
}

PupilListTreeWidget::~PupilListTreeWidget()
{
}


void PupilListTreeWidget::refreshPupilList()
{

    QList<QTreeWidgetItem *> selectedItemList = this->selectedItems();
    QString selectedItemIdString;
    if(!selectedItemList.empty()) {
        selectedItemIdString = selectedItemList.first()->data(0,Qt::UserRole).toString();
    }

    clear();
    setRootIsDecorated(false);

    QSqlQuery pupilQuery("SELECT pupilid, forename, surname FROM pupil ORDER BY surname ASC");
    if (pupilQuery.lastError().isValid()) qDebug() << "DB Error: 91 - " << pupilQuery.lastError();

    while (pupilQuery.next()) {
        QTreeWidgetItem *pupilItem = new QTreeWidgetItem(this);
        pupilItem->setFirstColumnSpanned ( true );
        pupilItem->setData(0, Qt::UserRole, pupilQuery.value(0).toString());
        pupilItem->setData(0, Qt::DisplayRole, pupilQuery.value(2).toString()+", "+pupilQuery.value(1).toString());
        pupilItem->setData(0, Qt::DecorationRole, QIcon(":/gfx/im-user.svg"));
    }

    if(topLevelItemCount()) {
        // 	find Selection
        int i;
        for (i=0; i<topLevelItemCount(); i++) {

            QTreeWidgetItem *item = topLevelItem(i);
            if(item->data(0, Qt::UserRole).toString() == selectedItemIdString) {
                setCurrentItem(item);
                break;
            }
        }
        myW->page_pupil->setEnabled(true);
    } else {
        myW->page_pupil->setDisabled(true);
    }

    //refresh menu
    myW->rightTabsChanged(1);
}

void PupilListTreeWidget::pupilListSelectionChanged(QTreeWidgetItem *current, QTreeWidgetItem */*previous*/)
{

    if(!selectedItems().empty()) {
        myW->stackedWidget_rightTabs->setCurrentIndex(1);
        bool ok;
        myW->tabWidget_pupil->setCurrentPupilId(current->data(0, Qt::UserRole).toString().toInt(&ok, 10));
        myW->tabWidget_pupil->loadPupilData();
    }

}

void PupilListTreeWidget::selectFirstItem()
{

    if(topLevelItemCount()) setCurrentItem(topLevelItem(0));
}

void PupilListTreeWidget::callPupilListContextMenu(const QPoint point)
{
    if(topLevelItemCount()) {
        pupilPopupMenu->popup(mapToGlobal(point));
    }
}

void PupilListTreeWidget::delCurrentPupil(bool firstItemSelection, bool menuRefresh)
{
    QList<QTreeWidgetItem *> selectedItemList = this->selectedItems();
    QString selectedItemIdString;
    if(!selectedItemList.empty()) {
        selectedItemIdString = selectedItemList.first()->data(0,Qt::UserRole).toString();
    }

    int ret = QMessageBox::warning(this, QString::fromUtf8(tr("Delete Pupil").toStdString().c_str()),
                                   QString::fromUtf8(tr("Do you really want to delete the pupil \"%1\" ?").arg(selectedItemList.first()->data(0,Qt::DisplayRole).toString().toUtf8().constData()).toStdString().c_str()),
                                   QMessageBox::Ok | QMessageBox::Cancel);
    if(ret == QMessageBox::Ok) {
        QSqlQuery delPupilQuery("DELETE FROM pupil WHERE pupilid = "+selectedItemIdString);
        if (delPupilQuery.lastError().isValid()) qDebug() << "DB Error: 92 - " << delPupilQuery.lastError();
        QSqlQuery delPupilQuery2("DELETE FROM note WHERE palid IN (SELECT palid FROM pupilatlesson WHERE pupilid ="+selectedItemIdString+")" );
        if (delPupilQuery2.lastError().isValid()) qDebug() << "DB Error: 93 - " << delPupilQuery2.lastError();
        QSqlQuery delPupilQuery3("DELETE FROM piece WHERE palid IN (SELECT palid FROM pupilatlesson WHERE pupilid ="+selectedItemIdString+")" );
        if (delPupilQuery3.lastError().isValid()) qDebug() << "DB Error: 94 - " << delPupilQuery3.lastError();
        QSqlQuery delPupilQuery1("DELETE FROM pupilatlesson WHERE pupilid = "+selectedItemIdString);
        if (delPupilQuery1.lastError().isValid()) qDebug() << "DB Error: 95 - " << delPupilQuery1.lastError();

        refreshPupilList();
        if(firstItemSelection)
            selectFirstItem();

        if(menuRefresh) //refresh menu
            myW->rightTabsChanged(1);
    }
}

void PupilListTreeWidget::archiveCurrentPupil()
{

    QList<QTreeWidgetItem *> selectedItemList = this->selectedItems();
    if(!selectedItemList.empty()) {

        QStringList completeLessonContentsList;

        QSqlQuery lessonCountQuery("SELECT count(*) FROM pupilatlesson WHERE pupilid="+selectedItemList.first()->data(0,Qt::UserRole).toString(), *myW->getMyDb()->getMyPupilDb());
        if (lessonCountQuery.lastError().isValid()) qDebug() << "DB Error: 96 - " << lessonCountQuery.lastError();
        lessonCountQuery.next();
        if(lessonCountQuery.value(0).toInt()) { /*only show acts if there is something*/

            QStringList myStateStringList;
            myStateStringList << tr("Planned") << tr("In Progress") << tr("Paused") << tr("Ready for Concert") << tr("Finished");

            QString lessonStrings("");
            int lessonCounter = 0;
            QStringList lessonContentsList;

            completeLessonContentsList << "<tr><td></td></tr><tr><td><h3><u>"+tr("Lesson notes and music pieces")+":</u></h3></td></tr><tr><td></td></tr>";
            //inactive lessons
            QSqlQuery palQuery("SELECT pal.palid, lln.lessonname, pal.startdate, pal.stopdate FROM pupilatlesson pal, lastlessonname lln WHERE pal.llnid = lln.llnid AND pal.pupilid = "+selectedItemList.first()->data(0,Qt::UserRole).toString()+" AND pal.stopdate <= '"+QDate::currentDate().toString(Qt::ISODate)+"'", *myW->getMyDb()->getMyPupilDb());
            if (palQuery.lastError().isValid()) qDebug() << "DB Error: 97 - " << palQuery.lastError();
            while (palQuery.next()) {
                lessonCounter++;
                lessonStrings += "<tr><td>"+QString::number(lessonCounter)+". "+palQuery.value(1).toString()+" ("+tr("from")+" "+QDate::fromString(palQuery.value(2).toString(), Qt::ISODate).toString("dd.MM.yyyy")+" "+tr("to")+" "+QDate::fromString(palQuery.value(3).toString(), Qt::ISODate).toString("dd.MM.yyyy")+")</td></tr>";

                QSqlQuery noteQuery("SELECT strftime(\"%d.%m.%Y\", n.date), n.content FROM note n, pupilatlesson pal WHERE pal.palid = "+palQuery.value(0).toString()+" AND pal.palid=n.palid AND pal.startdate <= n.date AND pal.stopdate >= n.date ORDER BY n.date ASC", *myW->getMyDb()->getMyPupilDb());
                if (noteQuery.lastError().isValid()) qDebug() << "DB Error: 98 - " << noteQuery.lastError();
                QString noteString("");
                if(noteQuery.next()) {
                    noteString += "<tr><td><b>"+tr("Lesson notes")+" ("+palQuery.value(1).toString()+" - "+tr("from")+" "+QDate::fromString(palQuery.value(2).toString(), Qt::ISODate).toString("dd.MM.yyyy")+" "+tr("to")+" "+QDate::fromString(palQuery.value(3).toString(), Qt::ISODate).toString("dd.MM.yyyy")+")</b></td></tr><tr><td><table style='text-align: left;' cellpadding='1' cellspacing='0' width='100%' style=' border-width:1px; border-style:solid;'>";
                    noteString += "<tr><td><u>"+tr("Date")+"</u></td><td><u>"+tr("Note")+"</u></td></tr>";
                    noteQuery.previous();
                    while (noteQuery.next()) {
                        noteString += "<tr><td>"+noteQuery.value(0).toString()+"</td><td>"+noteQuery.value(1).toString()+"</td></tr>";
                    }
                    noteString += "</table></td></tr><tr><td></td></tr>";
                    lessonContentsList << noteString;
                }


                QSqlQuery pieceQuery("SELECT p.title, p.genre, p.duration, strftime(\"%d.%m.%Y\", p.startdate), strftime(\"%d.%m.%Y\", p.stopdate), p.state FROM piece p, pupilatlesson pal WHERE pal.palid="+palQuery.value(0).toString()+" AND pal.palid=p.palid AND pal.startdate <= p.startdate AND pal.stopdate >= p.startdate ORDER BY p.startdate ASC", *myW->getMyDb()->getMyPupilDb());
                if (pieceQuery.lastError().isValid()) qDebug() << "DB Error: 99 - " << pieceQuery.lastError();
                QString pieceString("");
                if(pieceQuery.next()) {
                    pieceString += "<tr><td><b>"+tr("Music pieces")+" ("+palQuery.value(1).toString()+" - "+tr("from")+" "+QDate::fromString(palQuery.value(2).toString(), Qt::ISODate).toString("dd.MM.yyyy")+" "+tr("to")+" "+QDate::fromString(palQuery.value(3).toString(), Qt::ISODate).toString("dd.MM.yyyy")+")</b></td></tr><tr><td><table style='text-align: left;' cellpadding='1' cellspacing='0' width='100%' style=' border-width:1px; border-style:solid;'>";
                    pieceString += "<tr><td><u>"+tr("Title")+"</u></td><td><u>"+tr("Genre")+"</u></td><td><u>"+tr("Duration")+"</u></td><td><u>"+tr("Start")+"</u></td><td><u>"+tr("End")+"</u></td><td><u>"+tr("State")+"</u></td></tr>";
                    pieceQuery.previous();
                    while (pieceQuery.next()) {
                        pieceString += "<tr><td>"+pieceQuery.value(0).toString()+"</td><td>"+pieceQuery.value(1).toString()+"</td><td>"+pieceQuery.value(2).toString()+"</td><td>"+pieceQuery.value(3).toString()+"</td><td>"+pieceQuery.value(4).toString()+"</td><td>"+myStateStringList.at(pieceQuery.value(5).toInt())+"</td></tr>";
                    }
                    pieceString += "</table></td></tr><tr><td></td></tr>";
                    lessonContentsList << pieceString;
                }
            }

            //active lessons
            QSqlQuery palQuery1("SELECT pal.palid, l.lessonname, pal.startdate FROM pupilatlesson pal, lesson l WHERE pal.pupilid = "+selectedItemList.first()->data(0,Qt::UserRole).toString()+" AND pal.lessonid = l.lessonid AND pal.stopdate > '"+QDate::currentDate().toString(Qt::ISODate)+"' ORDER BY pal.startdate ASC", *myW->getMyDb()->getMyPupilDb());
            if (palQuery1.lastError().isValid()) qDebug() << "DB Error: 100 - " << palQuery1.lastError();
            while (palQuery1.next()) {
                lessonCounter++;
                lessonStrings += "<tr><td>"+QString::number(lessonCounter)+". "+palQuery1.value(1).toString()+" ("+tr("since")+" "+QDate::fromString(palQuery1.value(2).toString(), Qt::ISODate).toString("dd.MM.yyyy")+")</td></tr>";

                QSqlQuery noteQuery("SELECT strftime(\"%d.%m.%Y\", n.date), n.content FROM note n, pupilatlesson pal WHERE pal.palid = "+palQuery1.value(0).toString()+" AND pal.palid=n.palid AND pal.startdate <= n.date AND pal.stopdate >= n.date ORDER BY n.date ASC", *myW->getMyDb()->getMyPupilDb());
                if (noteQuery.lastError().isValid()) qDebug() << "DB Error: 101 - " << noteQuery.lastError();
                QString noteString("");
                if(noteQuery.next()) {
                    noteString += "<tr><td><b>"+tr("Lesson notes")+" ("+palQuery1.value(1).toString()+" - "+tr("since")+" "+QDate::fromString(palQuery1.value(2).toString(), Qt::ISODate).toString("dd.MM.yyyy")+")</b></td></tr><tr><td><table style='text-align: left;' cellpadding='1' cellspacing='0' width='100%' style=' border-width:1px; border-style:solid;'>";
                    noteString += "<tr><td><u>"+tr("Date")+"</u></td><td><u>"+tr("Note")+"</u></td></tr>";
                    noteQuery.previous();
                    while (noteQuery.next()) {
                        noteString += "<tr><td>"+noteQuery.value(0).toString()+"</td><td>"+noteQuery.value(1).toString()+"</td></tr>";
                    }
                    noteString += "</table></td></tr><tr><td></td></tr>";
                    lessonContentsList << noteString;
                }

                QSqlQuery pieceQuery("SELECT p.title, p.genre, p.duration, strftime(\"%d.%m.%Y\", p.startdate), strftime(\"%d.%m.%Y\", p.stopdate), p.state FROM piece p, pupilatlesson pal WHERE pal.palid="+palQuery1.value(0).toString()+" AND pal.palid=p.palid AND pal.startdate <= p.startdate AND pal.stopdate >= p.startdate ORDER BY p.startdate ASC", *myW->getMyDb()->getMyPupilDb());
                if (pieceQuery.lastError().isValid()) qDebug() << "DB Error: 102 - " << pieceQuery.lastError();
                QString pieceString("");
                if(pieceQuery.next()) {
                    pieceString += "<tr><td><b>"+tr("Music pieces")+" ("+palQuery1.value(1).toString()+" "+tr("since")+" "+QDate::fromString(palQuery1.value(2).toString(), Qt::ISODate).toString("dd.MM.yyyy")+")</b></td></tr><tr><td><table style='text-align: left;' cellpadding='1' cellspacing='0' width='100%' style=' border-width:1px; border-style:solid;'>";
                    pieceString += "<tr><td><u>"+tr("Title")+"</u></td><td><u>"+tr("Genre")+"</u></td><td><u>"+tr("Duration")+"</u></td><td><u>"+tr("Start")+"</u></td><td><u>"+tr("End")+"</u></td><td><u>"+tr("State")+"</u></td></tr>";
                    pieceQuery.previous();
                    while (pieceQuery.next()) {
                        pieceString += "<tr><td>"+pieceQuery.value(0).toString()+"</td><td>"+pieceQuery.value(1).toString()+"</td><td>"+pieceQuery.value(2).toString()+"</td><td>"+pieceQuery.value(3).toString()+"</td><td>"+pieceQuery.value(4).toString()+"</td><td>"+myStateStringList.at(pieceQuery.value(5).toInt())+"</td></tr>";
                    }
                    pieceString += "</table></td></tr><tr><td></td></tr>";
                    lessonContentsList << pieceString;
                }
            }
            completeLessonContentsList << "<tr><td><b>"+tr("The student took part in the following lessons")+":</b></td></tr>"+lessonStrings+"<tr><td><hr style='width: 100%; height: 1px;'></td></tr><tr><td></td></tr>";
            completeLessonContentsList << lessonContentsList.join("");
        }

        QStringList activityContentsList;

        QSqlQuery actCountQuery("SELECT count(*) FROM activity WHERE pupilid="+selectedItemList.first()->data(0,Qt::UserRole).toString(), *myW->getMyDb()->getMyPupilDb());
        if (actCountQuery.lastError().isValid()) qDebug() << "DB Error: 103 - " << actCountQuery.lastError();
        actCountQuery.next();
        if(actCountQuery.value(0).toInt()) { /*only show acts if there is something*/
            activityContentsList << "<tr><td></td></tr><tr><td></td></tr><tr><td><h3><u>"+tr("Activities of the student")+":</u></h3></td></tr><tr><td></td></tr>";
            //continous activity
            QSqlQuery contActQuery("SELECT desc, continousday, continoustime, strftime(\"%d.%m.%Y\", date), strftime(\"%d.%m.%Y\", continousstopdate) FROM activity WHERE pupilid="+selectedItemList.first()->data(0,Qt::UserRole).toString()+" AND ifcontinous=1 ORDER BY date DESC", *myW->getMyDb()->getMyPupilDb());
            if (contActQuery.lastError().isValid()) qDebug() << "DB Error: 104 - " << contActQuery.lastError();
            QString contActString("");
            contActString += "<tr><td><b>"+tr("Regular activities")+":</b></td></tr><tr><td><table style='text-align: left;' cellpadding='1' cellspacing='0' width='100%' style=' border-width:1px; border-style:solid;'>";
            contActString += "<tr><td><u>"+tr("Description")+"</u></td><td><u>"+tr("Weekday")+"</u></td><td><u>"+tr("Time")+"</u></td><td><u>"+tr("Start")+"</u></td><td><u>"+tr("End")+"</u></td></tr>";
            QStringList dayList;
            dayList << tr("Monday") << tr("Tuesday") << tr("Wednesday") << tr("Thursday") << tr("Friday") << tr("Saturday") << tr("Sunday") << QString::fromUtf8(tr("irregular").toStdString().c_str());
            while (contActQuery.next()) {
                contActString += "<tr><td>"+contActQuery.value(0).toString()+"</td><td>"+dayList.at(contActQuery.value(1).toInt())+"</td><td>"+contActQuery.value(2).toString()+"</td><td>"+contActQuery.value(3).toString()+"</td><td>"+contActQuery.value(4).toString()+"</td></tr>";
            }
            contActString += "</table></td></tr><tr><td></td></tr>";
            activityContentsList << contActString;

            //singular activity
            QStringList myTypeList;
            myTypeList << tr("Solo Recital") << tr("Ensemble Recital") << tr("Other");
            QSqlQuery singActQuery("SELECT desc, strftime(\"%d.%m.%Y\", date), noncontinoustype FROM activity WHERE pupilid="+selectedItemList.first()->data(0,Qt::UserRole).toString()+" AND ifcontinous=0 ORDER BY date DESC", *myW->getMyDb()->getMyPupilDb());
            if (singActQuery.lastError().isValid()) qDebug() << "DB Error: 105 - " << singActQuery.lastError();
            QString singActString("");
            singActString += "<tr><td><b>"+tr("Irregular activities")+":</b></td></tr><tr><td><table style='text-align: left;' cellpadding='1' cellspacing='0' width='100%' style=' border-width:1px; border-style:solid;'>";
            singActString += "<tr><td><u>"+tr("Description")+"</u></td><td><u>"+tr("Date")+"</u></td><td><u>"+tr("Type")+"</u></td></tr>";
            while (singActQuery.next()) {
                singActString += "<tr><td>"+singActQuery.value(0).toString()+"</td><td>"+singActQuery.value(1).toString()+"</td><td>"+myTypeList.at(singActQuery.value(2).toInt())+"</td></tr>";
            }
            singActString += "</table></td></tr><tr><td></td></tr>";
            activityContentsList << singActString;

        }

        QSqlQuery query("SELECT forename, surname, address, email, telefon, handy, birthday, notes, fathername, fatherjob, fathertelefon, mothername, motherjob, mothertelefon, firstlessondate, instrumenttype, instrumentsize, rentinstrumentdesc, rentinstrumentstartdate FROM pupil WHERE pupilid="+selectedItemList.first()->data(0,Qt::UserRole).toString(), *myW->getMyDb()->getMyPupilDb());
        if (query.lastError().isValid()) qDebug() << "DB Error: 106 - " << query.lastError();
        query.next();

        QString lessonyears("");
        //calculate years since first lesson
        if (query.value(14).toString() != "") {
            int daysFromFirstLesson = QDate::currentDate().daysTo(QDate::fromString(query.value(14).toString(), Qt::ISODate));
            lessonyears = QString::number(daysFromFirstLesson/365);
        }

        QString htmlString("<!DOCTYPE html PUBLIC '-//W3C//DTD HTML 4.01 Transitional//EN'> \
	<html> \
	<head> \
    <meta content='text/html; charset=utf-8' http-equiv='content-type'> \
    <title>Qupil - "+tr("Archive Entry")+" "+selectedItemList.first()->data(0,Qt::UserRole).toString()+"</title> \
	</head> \
	<body> \
	<table style='text-align: left; margin-left: auto; margin-right: auto; width: 100%;' border='0' cellpadding='0' cellspacing='0'> \
        <tr><td align='center' width='100%'><h1>Qupil "+RELEASE_STRING+" - "+tr("Archive Entry")+": "+query.value(0).toString()+" "+query.value(1).toString()+" (#"+selectedItemList.first()->data(0,Qt::UserRole).toString()+")</h2></td></tr> \
	<tr><td width='100%'></td></tr>\
	<tr><td >\
	<table style='text-align: left;' border='0' cellpadding='1' cellspacing='0' width='100%'> \
        <tr width='100%'><td colspan='4' rowspan='1' width='100%'><h2>"+tr("Date")+": "+QDate::currentDate().toString("dd.MM.yyyy")+"</h3></td></tr>\
		<tr><td colspan='4' rowspan='1'></td></tr>\
        <tr><td colspan='4' rowspan='1'><h3><u>"+tr("Personal data")+":</u></h3></td></tr>\
		<tr><td colspan='4' rowspan='1'></td></tr>\
        <tr><td width='100%'><b>"+tr("First name")+":</b></td><td width='100%'>"+query.value(0).toString()+"</td><td width='100%'><b>"+tr("Last name")+":</b></td><td width='100%'>"+query.value(1).toString()+"</td></tr>\
        <tr><td width='100%'><b>"+tr("Address")+":</b></td><td width='100%'>"+query.value(2).toString()+"</td><td width='100%'><b>"+tr("E-Mail")+":</b></td><td width='100%'>"+query.value(3).toString()+"</td></tr>\
        <tr><td width='100%'><b>"+tr("Phone")+":</b></td><td width='100%'>"+query.value(4).toString()+"</td><td width='100%'><b>"+tr("Mobile")+":</b></td><td width='100%'>"+query.value(5).toString()+"</td></tr>\
        <tr><td width='100%'><b>"+tr("Birthday")+":</b></td><td  colspan='3' rowspan='1'>"+QDate::fromString(query.value(6).toString(), Qt::ISODate).toString("dd.MM.yyyy")+"</td></tr>\
		<tr><td colspan='4' rowspan='1'></td></tr>\
		<tr width='100%'><td colspan='4' rowspan='1' width='100%'>\
		<table style='text-align: left; width: 100%;' border='0' cellpadding='0' cellspacing='0' width='100%'>\
        <tr width='100%'><td width='17%'><b>"+tr("Name (Father)")+":</b></td><td width='17%'>"+query.value(8).toString()+"</td><td width='17%'><b>"+tr("Job (Father)")+":</b></td><td width='17%'>"+query.value(9).toString()+"</td><td width='17%'><b>"+tr("Phone (Father)")+":</b></td><td width='17%'>"+query.value(10).toString()+"</td></tr>\
        <tr width='100%'><td width='17%'><b>"+tr("Name (Mother)")+":</b></td><td width='17%'>"+query.value(11).toString()+"</td><td width='17%'><b>"+tr("Job (Mother)")+":</b></td><td width='17%'>"+query.value(12).toString()+"</td><td width='17%'><b>"+tr("Phone (Mother)")+":</b></td><td width='17%'>"+query.value(13).toString()+"</td></tr>\
		</table></td></tr>\
		<tr><td colspan='4' rowspan='1'><hr style='width: 100%; height: 1px;'></td></tr>\
        <tr><td ><b>"+tr("Lesson since")+":</b></td><td colspan='3' rowspan='1'>"+QDate::fromString(query.value(14).toString(), Qt::ISODate).toString("dd.MM.yyyy")+" ("+tr("total")+" "+lessonyears.remove("-")+" "+tr("Years")+")</td></tr>\
		<tr><td colspan='4' rowspan='1'><hr style='width: 100%; height: 1px;'></td></tr>\
        <tr><td width='100%'><b>"+tr("Instrument")+":</b></td><td width='100%'>"+query.value(15).toString()+"</td><td width='100%'><b>"+tr("Size")+":</b></td><td width='100%'>"+query.value(16).toString()+"</td></tr>\
        <tr style='width: 100%;'><td style='width: 100%;'><b>"+tr("Rental instrument description")+":</b></td><td width='100%'>"+query.value(17).toString()+"</td><td width='100%'><b>"+tr("Rented since")+":</b></td><td width='100%'>"+query.value(18).toString()+"</td></tr>\
		<tr><td colspan='4' rowspan='1'><hr style='width: 100%; height: 1px;'></td></tr>\
		<tr><td colspan='4' rowspan='1'>\
		<table style='text-align: left; width: 100%;' border='0' cellpadding='0' cellspacing='0'>\
        <tr><td style='vertical-align: top;'><b>"+tr("Notes")+": </b></td><td style='vertical-align: top;'>"+query.value(7).toString()+"</td></tr>\
		</table></td></tr>\
	</table></td></tr>"+activityContentsList.join("")+"\
	<tr><td></td></tr>"+completeLessonContentsList.join("")+"\
	<tr><td></td></tr>\
	<tr><td></td></tr>\
        <tr><td align='center'><i>Qupil "+RELEASE_STRING+" - &copy;2006-"+QDate::currentDate().toString("yyyy")+" - Felix Hammer - qupil.de</i></td></tr>\
	</table>\
	</body>\
	</html>");

        QSqlQuery pupilArchiveQuery(*myW->getMyDb()->getMyPupilDb());
        pupilArchiveQuery.prepare("REPLACE INTO pupilarchive ( pupilid, forename, surname, data ) VALUES ( "+selectedItemList.first()->data(0,Qt::UserRole).toString()+", (SELECT forename FROM pupil WHERE pupilid = "+selectedItemList.first()->data(0,Qt::UserRole).toString()+"), (SELECT surname FROM pupil WHERE pupilid = "+selectedItemList.first()->data(0,Qt::UserRole).toString()+"), ?)" );
        pupilArchiveQuery.addBindValue(htmlString);
        pupilArchiveQuery.exec();
        if (pupilArchiveQuery.lastError().isValid()) qDebug() << "DB Error: 107 - " << pupilArchiveQuery.lastError();

        myW->treeWidget_pupilArchiveList->refreshPupilArchiveList(selectedItemList.first()->data(0,Qt::UserRole).toInt());
        myW->comboBox_leftListMode->setCurrentIndex(2);
    }
}

void PupilListTreeWidget::archiveAndDelCurrentPupil()
{
    archiveCurrentPupil();
    delCurrentPupil(false, false);

// 	refresh menu
    myW->rightTabsChanged(2);
}
