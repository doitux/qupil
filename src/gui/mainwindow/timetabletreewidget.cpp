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
#include "timetabletreewidget.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"
#include "configfile.h"
#include <QtSql>

TimeTableTreeWidget::TimeTableTreeWidget(QWidget *tab)
    : QTreeWidget(tab), headerSectionIndent(10)
{
    lessonPopupMenu = new QMenu();
    delLesson = new QAction(QIcon(":/gfx/x-office-calendar.svg"), QString::fromUtf8(tr("Unterricht löschen").toStdString().c_str()), lessonPopupMenu);
    lessonPopupMenu->addAction(delLesson);

    connect( this, SIGNAL ( currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT ( timeTableSelectionChanged(QTreeWidgetItem*, QTreeWidgetItem*)) );
    connect( this, SIGNAL ( customContextMenuRequested(const QPoint) ), this, SLOT ( callTimeTableContextMenu( const QPoint) ) );
    connect( delLesson, SIGNAL ( triggered() ), this, SLOT ( delCurrentLesson() ) );
}

TimeTableTreeWidget::~TimeTableTreeWidget()
{
}


void TimeTableTreeWidget::refreshTimeTable()
{

    QList<QTreeWidgetItem *> selectedItemList = this->selectedItems();
    QString selectedItemIdString;
    if(!selectedItemList.empty()) {
        selectedItemIdString = selectedItemList.first()->data(0,Qt::UserRole).toString();
    }

    int pupilCounter = 0;
    int lessonCounter = 0;

    int minimumTreeWidgetWidth = 0;

    clear();
    setRootIsDecorated(false);

    QStringList dayList;
    dayList << tr("Montag") << tr("Dienstag") << tr("Mittwoch") << tr("Donnerstag") << tr("Freitag") << tr("Samstag") << tr("Sonntag") << QString::fromUtf8(tr("unregelmäßig").toStdString().c_str());

    QString longestLessonDescription = "";
    int longestLessonDescriptionLength = 0;
    int longestLessonDescriptionColumnWidth = 0;

    int i;
    for(i = 0; i < 8; i++)  {

        QSqlQuery query;
        if(i < 7) {
            query.exec("SELECT lessonid, lessonname, lessonstarttime, lessonstoptime FROM lesson WHERE state = 1 AND lessonday="+QString("%1").arg(i,10)+" ORDER BY lessonstarttime ASC");
            if (query.lastError().isValid()) qDebug() << "DB Error: 134 - " << query.lastError();
        } else {
            query.exec("SELECT lessonid, lessonname FROM lesson WHERE state = 1 AND unsteadylesson = 1");
            if (query.lastError().isValid()) qDebug()  << "DB Error: 135 - " << query.lastError();
        }

        if(query.next()) {

            QTreeWidgetItem *dayItem = new QTreeWidgetItem(this);
            dayItem->setFirstColumnSpanned ( true );
            dayItem->setData(0, Qt::UserRole, QString("d%1").arg(-8+i));
            dayItem->setData(0, Qt::DisplayRole, dayList.at(i));
            QFont dayFont;
            QFont tempFont;
            tempFont.setBold(true);
            tempFont.setPointSize(dayItem->font(0).pointSize()+1);
            dayFont.setBold(true);
            dayFont.setPointSize(dayItem->font(0).pointSize()+3);
            dayItem->setFont(0, dayFont);
            dayItem->setBackground(0, QBrush(QColor(QString(myConfig->readConfigString("TimeTableDayBColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTableDayBColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTableDayBColor").c_str()).section(",",2,2).toInt())));
            dayItem->setForeground(0, QBrush(QColor(QString(myConfig->readConfigString("TimeTableDayTColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTableDayTColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTableDayTColor").c_str()).section(",",2,2).toInt())));
            dayItem->setTextAlignment(0,Qt::AlignHCenter);
            dayItem->setFlags(Qt::NoItemFlags);
            query.previous();

            QFontMetrics fm(tempFont);
            int pixelsWide = fm.width("99:99 - 99:99");
            //set correct size of first column
            int firstColumnWidth = pixelsWide+13;
            setColumnWidth(0,firstColumnWidth);
            //set first half of the Width
            minimumTreeWidgetWidth = firstColumnWidth;
        }

        while (query.next()) {
            lessonCounter++;
            QTreeWidgetItem *item = new QTreeWidgetItem(this);
            item->setData(0, Qt::UserRole, "l"+query.value(0).toString());

            if(i < 7) {
                //lesson time
                item->setData(0, Qt::DisplayRole, query.value(2).toString()+" - "+query.value(3).toString());
                QFont tempFont;
                tempFont.setBold(true);
                tempFont.setPointSize(item->font(0).pointSize()+1);
                item->setFont(0, tempFont);
                //lesson description
                item->setData(1, Qt::DisplayRole, query.value(1).toString());
                item->setBackground(0, QBrush(QColor(QString(myConfig->readConfigString("TimeTableLessonBColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTableLessonBColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTableLessonBColor").c_str()).section(",",2,2).toInt())));
                item->setBackground(1, QBrush(QColor(QString(myConfig->readConfigString("TimeTableLessonBColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTableLessonBColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTableLessonBColor").c_str()).section(",",2,2).toInt())));
                item->setForeground(0, QBrush(QColor(QString(myConfig->readConfigString("TimeTableLessonTColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTableLessonTColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTableLessonTColor").c_str()).section(",",2,2).toInt())));
                item->setForeground(1, QBrush(QColor(QString(myConfig->readConfigString("TimeTableLessonTColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTableLessonTColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTableLessonTColor").c_str()).section(",",2,2).toInt())));
                QSize size = item->sizeHint(0);
                size.rheight() += 25;
                item->setSizeHint ( 0, size );
                item->setSizeHint ( 1, size );
            } else {
                item->setFirstColumnSpanned ( true );
                item->setData(0, Qt::DisplayRole, query.value(1).toString());
                item->setBackground(0, QBrush(QColor(QString(myConfig->readConfigString("TimeTableLessonBColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTableLessonBColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTableLessonBColor").c_str()).section(",",2,2).toInt())));
                item->setForeground(0, QBrush(QColor(QString(myConfig->readConfigString("TimeTableLessonTColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTableLessonTColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTableLessonTColor").c_str()).section(",",2,2).toInt())));
                QSize size = item->sizeHint(0);
                size.rheight() += 25;
                item->setSizeHint ( 0, size );
            }

            //check for the item lenght compare and set the longest lenth to calculate the needed space with font metrics
            if(item->data(1, Qt::DisplayRole).toString().length() > longestLessonDescriptionLength) {
                longestLessonDescriptionLength = item->data(1, Qt::DisplayRole).toString().length();
                longestLessonDescription = item->data(1, Qt::DisplayRole).toString();

                QFontMetrics fm(item->font(1));
                longestLessonDescriptionColumnWidth = fm.width(longestLessonDescription);
            }

            QSqlQuery pupilQuery("SELECT pal.palid, p.forename, p.surname FROM pupilatlesson pal, pupil p WHERE pal.lessonid= "+query.value(0).toString()+" AND p.pupilid = pal.pupilid AND pal.stopdate > '"+QDate::currentDate().toString(Qt::ISODate)+"' ORDER BY p.surname ASC");
            if (pupilQuery.lastError().isValid()) qDebug() << "DB Error: 136 - " << pupilQuery.lastError();

            while (pupilQuery.next()) {
                pupilCounter++;
                QTreeWidgetItem *pupilItem = new QTreeWidgetItem(item);
                pupilItem->setFirstColumnSpanned ( true );
                pupilItem->setData(0, Qt::UserRole, "p"+pupilQuery.value(0).toString());
                pupilItem->setData(0, Qt::DisplayRole, pupilQuery.value(2).toString()+", "+pupilQuery.value(1).toString());
                pupilItem->setData(0, Qt::DecorationRole, QIcon(":/gfx/im-user.svg"));
                pupilItem->setBackground(0, QBrush(QColor(QString(myConfig->readConfigString("TimeTablePupilBColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTablePupilBColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTablePupilBColor").c_str()).section(",",2,2).toInt())));
                pupilItem->setForeground(0, QBrush(QColor(QString(myConfig->readConfigString("TimeTablePupilTColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTablePupilTColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTablePupilTColor").c_str()).section(",",2,2).toInt())));
                QSize size = pupilItem->sizeHint(0);
                size.rheight() += 20;
                pupilItem->setSizeHint ( 0, size );
            }
            item->setExpanded(true);

        }
    }

    if(topLevelItemCount()) {

        resizeColumnToContents(1);

        //adjust parent stackwidget to fit the needed space depending on system font size (calculated above)
        minimumTreeWidgetWidth += longestLessonDescriptionColumnWidth + 35;
        if(minimumTreeWidgetWidth > 250) {
            this->setMinimumWidth(minimumTreeWidgetWidth);
            this->setMaximumWidth(minimumTreeWidgetWidth);
            myW->comboBox_leftListMode->setMinimumWidth(minimumTreeWidgetWidth);
            myW->comboBox_leftListMode->setMaximumWidth(minimumTreeWidgetWidth);
            myW->stackedWidget_leftList->setMinimumWidth(minimumTreeWidgetWidth);
            myW->stackedWidget_leftList->setMaximumWidth(minimumTreeWidgetWidth);
        }
        // 	find Selection
        int i;
        for (i=0; i<topLevelItemCount(); i++) {

            QTreeWidgetItem *item = topLevelItem(i);
            if(item->data(0, Qt::UserRole).toString() == selectedItemIdString) {
                setCurrentItem(item);
                scrollToItem(item, QAbstractItemView::PositionAtCenter);
                break;
            } else {
                if(item->childCount()) {
                    int j;
                    for (j=0; j<item->childCount(); j++) {
                        QTreeWidgetItem *childItem = item->child(j);
                        if(childItem->data(0, Qt::UserRole).toString() == selectedItemIdString) {
                            setCurrentItem(childItem);
                            scrollToItem(childItem, QAbstractItemView::PositionAtCenter);
                            break;
                        }
                    }
                }
            }
        }
        myW->page_lesson->setEnabled(true);
    } else {
        myW->page_lesson->setDisabled(true);
    }

    myW->refreshTimeTableStats(lessonCounter, pupilCounter);

}

void TimeTableTreeWidget::timeTableSelectionChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{

    if(!selectedItems().empty()) {
        if(current->data(0, Qt::UserRole).toString().startsWith("d")) {
            if(indexOfTopLevelItem(current) > indexOfTopLevelItem(previous))
                setCurrentItem(itemBelow(current));
            else if (indexOfTopLevelItem(current) > 0)
                setCurrentItem(itemAbove(current));
            else
                setCurrentItem(itemBelow(current));
        }

        //show lesson tabs if a lesson is selected
        if(current->data(0, Qt::UserRole).toString().startsWith("l")) {

            myW->stackedWidget_rightTabs->setCurrentIndex(0);
            bool ok;
            myW->tabWidget_lesson->setCurrentLessonId(current->data(0, Qt::UserRole).toString().remove(0,1).toInt(&ok, 10));
            myW->tabWidget_lesson->loadLessonData();
        }

        //show pupil tabs if a pupil is selected
        if(current->data(0, Qt::UserRole).toString().startsWith("p")) {

            QSqlQuery pupilQuery("SELECT pupilid FROM pupilatlesson WHERE palid = "+current->data(0, Qt::UserRole).toString().remove(0,1));
            if (pupilQuery.lastError().isValid()) qDebug() << "DB Error: 137 - " << pupilQuery.lastError();
            pupilQuery.next();
            myW->stackedWidget_rightTabs->setCurrentIndex(1);
            myW->tabWidget_pupil->setCurrentPupilId(pupilQuery.value(0).toInt());
            myW->tabWidget_pupil->loadPupilData();
            myW->comboBox_palPiecesSelection->setCurrentIndex(myW->comboBox_palPiecesSelection->findData(current->data(0, Qt::UserRole).toString().remove(0,1)));
            myW->comboBox_palNotesSelection->setCurrentIndex(myW->comboBox_palNotesSelection->findData(current->data(0, Qt::UserRole).toString().remove(0,1)));
            myW->textEdit_palNotes->clear();
        }
    }
}

void TimeTableTreeWidget::selectFirstLessonItem()
{

    int i;
    for (i=0; i<topLevelItemCount(); i++) {

        QTreeWidgetItem *item = topLevelItem(i);
        bool ok;
        // 		qDebug() << item->data(0, Qt::UserRole).toString();
        if(item->data(0, Qt::UserRole).toString().remove(0,1).toInt(&ok, 10) > 0) {
            setCurrentItem(item);
            break;
        }
    }
}

void TimeTableTreeWidget::callTimeTableContextMenu(const QPoint point)
{
    if(topLevelItemCount()) {
        QList<QTreeWidgetItem *> selectedItemList = this->selectedItems();
        QString selectedItemIdString;
        if(!selectedItemList.empty()) {
            if(selectedItemList.first()->data(0,Qt::UserRole).toString().startsWith("l")) {
                lessonPopupMenu->popup(mapToGlobal(point));
            }
        }
    }

}

void TimeTableTreeWidget::delCurrentLesson()
{
    QList<QTreeWidgetItem *> selectedItemList = this->selectedItems();
    QString selectedItemIdString;
    if(!selectedItemList.empty()) {
        selectedItemIdString = selectedItemList.first()->data(0,Qt::UserRole).toString().remove(0,1);

    }

    int ret = QMessageBox::warning(this, QString::fromUtf8(tr("Qupil - Unterricht löschen").toStdString().c_str()),
                                   QString::fromUtf8(tr("Möchten Sie den ausgewählten Unterricht wirklich löschen?").toStdString().c_str()),
                                   QMessageBox::Ok | QMessageBox::Cancel);
    if(ret == QMessageBox::Ok) {

        //remove Pupil from lesson before delete
        QListIterator<QStringList> it(myW->tabWidget_lesson->getPupilPalListBeforeEdit());
        while (it.hasNext()) {

            QStringList stringList(it.next());

            QSqlQuery query3("SELECT startdate FROM pupilatlesson WHERE palid = "+stringList.at(1));
            if (query3.lastError().isValid()) {
                qDebug() << "DB Error: 138 - " << query3.lastError();
            }
            query3.next();
            // 			check if startdate is minimum a month ago until remove now. If yes update and remember
            if(QDate::fromString(query3.value(0).toString(), Qt::ISODate).daysTo(QDate::currentDate()) > 30) {

                QSqlQuery query1("INSERT INTO lastlessonname ( lessonname ) VALUES ( '"+myW->tabWidget_lesson->getAutoLessonNameBeforeEdit()+"' )");
                if (query1.lastError().isValid()) {
                    qDebug() << "DB Error: 139 - " << query1.lastError();
                }
                QSqlQuery query2("SELECT last_insert_rowid()");
                if (query2.lastError().isValid()) {
                    qDebug() << "DB Error: 140 - " << query2.lastError();
                }
                query2.next();

                QSqlQuery query;
                query.prepare("UPDATE pupilatlesson SET stopdate = ?, llnid = ? WHERE palid = ?");
                query.addBindValue(QDate::currentDate().toString(Qt::ISODate));
                query.addBindValue(query2.value(0).toInt());
                query.addBindValue(stringList.at(1).toInt());
                query.exec();
                if (query.lastError().isValid()) {
                    qDebug() << "DB Error: 141 - " << query.lastError();
                }
            } else { // If no remove completly because time was to short
                QSqlQuery query1("DELETE FROM pupilatlesson WHERE palid = "+stringList.at(1));
                if (query1.lastError().isValid()) {
                    qDebug() << "DB Error: 142 - " << query1.lastError();
                }
            }
        }

        QSqlQuery thisLessonPalCount("SELECT count(*) FROM pupilatlesson WHERE lessonid = "+selectedItemIdString, *myW->getMyDb()->getMyPupilDb());
        if (thisLessonPalCount.lastError().isValid()) qDebug() << "DB Error: 143 - " << thisLessonPalCount.lastError();
        thisLessonPalCount.next();

        if(thisLessonPalCount.value(0).toInt()) { /*dont delete just update inactive because there are currently pal connections to this lesson */
            QSqlQuery delLessonQuery("UPDATE lesson SET state = 0 WHERE lessonid="+selectedItemIdString);
            if (delLessonQuery.lastError().isValid()) qDebug() << "DB Error: 144 - " << delLessonQuery.lastError();
            refreshTimeTable();
            selectFirstLessonItem();
        } else { /* comletely delete because there arent any pal connections to this lesson */
            QSqlQuery delLessonQuery("DELETE FROM lesson WHERE lessonid="+selectedItemIdString);
            if (delLessonQuery.lastError().isValid()) qDebug() << "DB Error: 145 - " << delLessonQuery.lastError();
            refreshTimeTable();
            selectFirstLessonItem();
        }

        //refresh menu
        myW->rightTabsChanged(0);
    }
}

void TimeTableTreeWidget::selectFirstTodayLessonItem()
{

    QSqlQuery query("SELECT lessonid FROM lesson WHERE lessonday= "+QString::number(QDate::currentDate().dayOfWeek()-1)+"  AND lessonstarttime <= '"+QTime::currentTime().toString("hh:mm")+"' AND lessonstoptime > '" +QTime::currentTime().toString("hh:mm")+"' AND state=1");
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 169 - " << query.lastError();
    }
    if(query.next()) {
        if(topLevelItemCount()) {
            int i;
            for (i=0; i<topLevelItemCount(); i++) {

                QTreeWidgetItem *item = topLevelItem(i);
                if(item->data(0, Qt::UserRole).toString() == "l"+query.value(0).toString()) {
                    //if lessonitem has a child --> select the child
                    if(item->childCount()) {
                        QTreeWidgetItem *child = item->child(0);
                        setCurrentItem(child);
                    } else {
                        setCurrentItem(item);
                    }
                    break;
                }
            }
        }
    } else {

        QSqlQuery query2("SELECT lessonid FROM lesson WHERE lessonday= "+QString::number(QDate::currentDate().dayOfWeek()-1)+" AND state=1 ORDER BY lessonstarttime ASC");
        if (query2.lastError().isValid()) {
            qDebug() << "DB Error: 232 - " << query2.lastError();
        }
        if(query2.next()) {
            if(topLevelItemCount()) {
                int i;
                for (i=0; i<topLevelItemCount(); i++) {
                    QTreeWidgetItem *item = topLevelItem(i);

                    if(item->data(0, Qt::UserRole).toString() == "l"+query2.value(0).toString()) {
                        //if lessonitem has a child --> select the child
                        if(item->childCount()) {
                            QTreeWidgetItem *child = item->child(0);
                            setCurrentItem(child);
                        } else {
                            setCurrentItem(item);
                        }
                        break;
                    }
                }
            }
        } else {
            selectFirstLessonItem();
        }
    }
}
