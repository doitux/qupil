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
#include "lessontabwidget.h"
#include "mainwindowimpl.h"
#include "mydbhandler.h"
#include "configfile.h"
#include <QtSql>

LessonTabWidget::LessonTabWidget(QWidget *tab)
    : QTabWidget(tab), saveOk(true)
{

}

LessonTabWidget::~LessonTabWidget()
{
}

void LessonTabWidget::init()
{

    connect( myW->pushButton_saveLessonDetails, SIGNAL( clicked() ), this, SLOT( saveLessonDetails() ) );
    connect( myW->pushButton_addPupil, SIGNAL( clicked() ), this, SLOT( addPupilToLesson() ) );
    connect( myW->pushButton_delPupil, SIGNAL( clicked() ), this, SLOT( delPupilFromLesson() ) );
    connect( myW->radioButton_singleLesson, SIGNAL( clicked() ), this, SLOT( setSingleLesson() ) );
    connect( myW->radioButton_groupLesson, SIGNAL( clicked() ), this, SLOT( setGroupLesson() ) );
    connect( myW->radioButton_ensembleLesson, SIGNAL( clicked() ), this, SLOT( setEnsembleLesson() ) );
    connect( myW->radioButton_steadyLessonDate, SIGNAL( clicked() ), this, SLOT( setSteadyLessonDate() ) );
    connect( myW->radioButton_unsteadyLessonDate, SIGNAL( clicked() ), this, SLOT( setUnsteadyLessonDate() ) );
    connect( myW->timeEdit_lessonStartTime, SIGNAL( timeChanged ( QTime ) ), this, SLOT ( lessonStartTimeChanged( QTime ) ) );
    connect( myW->timeEdit_lessonStopTime, SIGNAL( timeChanged ( QTime ) ), this, SLOT ( lessonStopTimeChanged( QTime ) ) );
    connect( myW->spinBox_lessonDuration, SIGNAL( valueChanged( int ) ), this, SLOT ( lessonDurationChanged() ) );
    connect( myW->comboBox_lessonLocation, SIGNAL( currentIndexChanged( int ) ), this, SLOT ( lessonLocationChanged() ) );
    connect( myW->treeWidget_availablePupil, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(addPupilToLesson()));
    connect( myW->treeWidget_pupilAtLesson, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(delPupilFromLesson()));

}

void LessonTabWidget::loadComboBoxItems()
{

    myW->comboBox_lessonLocation->clear();

    QStringList myLessonLocationStringList;
    std::list<std::string> locationList = myConfig->readConfigStringList("LessonLocationList");
    std::list<std::string>::iterator it2;
    for(it2= locationList.begin(); it2 != locationList.end(); it2++) {
        myLessonLocationStringList << QString::fromUtf8(it2->c_str());
    }
    myW->comboBox_lessonLocation->insertItems ( 0, myLessonLocationStringList );
}

void LessonTabWidget::setMyW ( mainWindowImpl* theValue )
{
    myW = theValue;

}

void LessonTabWidget::setMyConfig( ConfigFile* theValue )
{
    myConfig = theValue;
}

void LessonTabWidget::loadLessonData()
{
    loadComboBoxItems();
    loadLessonDetails();
}

void LessonTabWidget::loadLessonDetails()
{

    tempPupilList.clear();
    pupilPalListBeforeEdit.clear();
    pupilListBeforeEdit.clear();

    QSqlQuery query("SELECT type, autolessonname, lessonname, unsteadylesson, lessonstarttime, lessonstoptime, lessonday, lessonlocation FROM lesson WHERE lessonid="+QString::number(currentLessonId,10), *myW->getMyDb()->getMyPupilDb());
    if (query.lastError().isValid()) qDebug() << "DB Error: 49 - " << query.lastError();
    query.next();

    //set ensemble type radiobutton
    switch (query.value(0).toInt()) {
    case 1:
        myW->radioButton_singleLesson->setChecked(true);
        break;
    case 2:
        myW->radioButton_groupLesson->setChecked(true);
        break;
    case 3:
        myW->radioButton_ensembleLesson->setChecked(true);
        break;
    }

    //set naming type radiobutton
    switch (query.value(1).toInt()) {
    case 0: {
        myW->radioButton_manualLessonName->setChecked(true);
        myW->lineEdit_manualLessonName->setText(query.value(2).toString());
    }
    break;
    case 1: {
        myW->radioButton_autoLessonName->setChecked(true);
        myW->lineEdit_manualLessonName->setText("");
    }
    break;
    }

    //set regular or unsteady lesson
    switch (query.value(3).toInt()) {
    case 0: {
        myW->radioButton_steadyLessonDate->setChecked(true);
        myW->comboBox_lessonDay->setCurrentIndex(query.value(6).toInt());
        myW->timeEdit_lessonStartTime->setTime(QTime::fromString(query.value(4).toString(),"hh:mm"));
        myW->timeEdit_lessonStopTime->setTime(QTime::fromString(query.value(5).toString(),"hh:mm"));
        myW->spinBox_lessonDuration->setValue(QTime::fromString(query.value(4).toString(),"hh:mm").secsTo(QTime::fromString(query.value(5).toString(),"hh:mm"))/60);
    }
    break;
    case 1: {
        myW->radioButton_unsteadyLessonDate->setChecked(true);
        myW->comboBox_lessonDay->setCurrentIndex(0);
        myW->timeEdit_lessonStartTime->setTime(QTime::fromString("00:00", "hh:mm"));
        myW->timeEdit_lessonStopTime->setTime(QTime::fromString("00:00", "hh:mm"));
        myW->spinBox_lessonDuration->setValue(0);
    }
    break;
    }

    int pos = myW->comboBox_lessonLocation->findText(query.value(7).toString(), Qt::MatchExactly );
    if(pos == -1) {
        myW->comboBox_lessonLocation->addItem(query.value(7).toString());
        pos = myW->comboBox_lessonLocation->findText(query.value(7).toString(), Qt::MatchExactly );
    }
    myW->comboBox_lessonLocation->setCurrentIndex(pos);

    //load listView_pupilAtLesson
    myW->treeWidget_pupilAtLesson->clear();
    QSqlQuery pupilAtLessonQuery("SELECT p.pupilid, p.forename, p.surname, pal.palid FROM pupilatlesson pal, pupil p WHERE pal.lessonid="+QString::number(currentLessonId,10)+" AND p.pupilid = pal.pupilid AND pal.stopdate > '"+QDate::currentDate().toString(Qt::ISODate)+"' ORDER BY p.surname ASC", *myW->getMyDb()->getMyPupilDb());
    if (pupilAtLessonQuery.lastError().isValid()) qDebug() << "DB Error: 50 - " << pupilAtLessonQuery.lastError();
    while (pupilAtLessonQuery.next()) {
        QTreeWidgetItem *pupilAtLessonItem = new QTreeWidgetItem(myW->treeWidget_pupilAtLesson);
        pupilAtLessonItem->setData(0, Qt::UserRole, pupilAtLessonQuery.value(0).toString());
        pupilAtLessonItem->setData(0, Qt::DisplayRole, pupilAtLessonQuery.value(2).toString()+", "+pupilAtLessonQuery.value(1).toString());
        // 		build pupilPalListBeforeEdit for saving procedure
        QStringList tmp;
        tmp << pupilAtLessonQuery.value(0).toString() << pupilAtLessonQuery.value(3).toString();
        pupilPalListBeforeEdit << tmp;
        // 		build pupilListBeforeEdit for saving procedure
        pupilListBeforeEdit << pupilAtLessonQuery.value(0).toString();
        // 		build tempPupilList for editing
        tempPupilList << pupilAtLessonQuery.value(0).toString();
    }

    //load listView_availablePupil
    refreshAvailablePupilList();

    //depend on naming type read autoLessonNameBeforeEdit
    if(query.value(1).toInt()) {
        autoLessonNameBeforeEdit = updateAutoLessonName();
    } else {
        autoLessonNameBeforeEdit = query.value(2).toString();
    }
    updateAutoLessonName();
}

void LessonTabWidget::refreshAvailablePupilList()
{

    myW->treeWidget_availablePupil->clear();
    QSqlQuery availablePupilQuery("SELECT p.pupilid, p.forename, p.surname FROM pupil p WHERE p.pupilid NOT IN ("+tempPupilList.join(", ")+") ORDER BY p.surname ASC", *myW->getMyDb()->getMyPupilDb());
    if (availablePupilQuery.lastError().isValid()) qDebug() << "DB Error: 51 - " << availablePupilQuery.lastError();
    while (availablePupilQuery.next()) {
        QTreeWidgetItem *availablePupilItem = new QTreeWidgetItem(myW->treeWidget_availablePupil);
        availablePupilItem->setData(0, Qt::UserRole, availablePupilQuery.value(0).toString());
        availablePupilItem->setData(0, Qt::DisplayRole, availablePupilQuery.value(2).toString()+", "+availablePupilQuery.value(1).toString());
    }
}

void LessonTabWidget::saveLessonDetails()
{

    saveOk = true;

    QString lessonDayString("");
    if(myW->radioButton_steadyLessonDate->isChecked()) lessonDayString = ", lessonday = ?";

    QSqlQuery query(*myW->getMyDb()->getMyPupilDb());
    query.prepare("UPDATE lesson SET type = ?, autolessonname = ?, lessonname = ?, unsteadylesson = ?, lessonstarttime = ?, lessonstoptime = ?"+lessonDayString+", lessonlocation = ? WHERE lessonid="+QString::number(currentLessonId,10));

    //save lesson type
    int lessonType;
    if(myW->radioButton_singleLesson->isChecked()) lessonType = 1;
    else if(myW->radioButton_groupLesson->isChecked()) lessonType = 2;
    else lessonType = 3;
    query.addBindValue(lessonType);

    //save naming type and string
    int namingType;
    QString name;
    if(myW->radioButton_manualLessonName->isChecked()) {
        namingType = 0;
        name = myW->lineEdit_manualLessonName->text();
    } else {
        namingType = 1;
        name = myW->label_autoLessonName->text();
    }
    query.addBindValue(namingType);
    query.addBindValue(name);

    //save regular or unsteady lesson and details
    int unsteadyLesson;
    QString lessonStartTime;
    QString lessonStopTime;
    int lessonDay;
    if(myW->radioButton_steadyLessonDate->isChecked()) {
        unsteadyLesson = 0;
        lessonStartTime = myW->timeEdit_lessonStartTime->time().toString("hh:mm");
        lessonStopTime = myW->timeEdit_lessonStopTime->time().toString("hh:mm");
        lessonDay = myW->comboBox_lessonDay->currentIndex();
    } else {
        unsteadyLesson = 1;
        lessonStartTime = "";
        lessonStopTime = "";
        lessonDay = -1;
    }
    query.addBindValue(unsteadyLesson);
    query.addBindValue(lessonStartTime);
    query.addBindValue(lessonStopTime);
    if(lessonDay != -1)
        query.addBindValue(lessonDay);

    QString lessonLocation = myW->comboBox_lessonLocation->currentText();
    query.addBindValue(lessonLocation);

    query.exec();
    if (query.lastError().isValid()) qDebug() << "DB Error: 52 - " << query.lastError();

    //save pupilAtLesson Data
    if(myW->radioButton_singleLesson->isChecked() && myW->treeWidget_pupilAtLesson->topLevelItemCount() > 1) {
        QMessageBox::information(this, tr("Qupil"),
                                 QString::fromUtf8(tr("Sie können einem Einzelunterricht nicht mehr als einen Schüler zuweisen\n"
                                         "Bitte ändern Sie die Schüleranzahl um zu speichern!").toStdString().c_str()),
                                 QMessageBox::Ok);
        saveOk = false;
    } else {
        //1. see if previous pupils are removed --> UPDATE stopdate
        QListIterator<QStringList> it(pupilPalListBeforeEdit);
        while (it.hasNext()) {
            QStringList stringList(it.next());
            // 		pupil was removed --> UPDATE stopdate
            if(!tempPupilList.contains(stringList.at(0))) {

                QSqlQuery query3("SELECT startdate FROM pupilatlesson WHERE palid = "+stringList.at(1));
                if (query3.lastError().isValid()) {
                    qDebug() << "DB Error: 53 - " << query3.lastError();
                }
                query3.next();
                //check if startdate is minimum a month ago until remove now. If yes update and remember
                if(QDate::fromString(query3.value(0).toString(), Qt::ISODate).daysTo(QDate::currentDate()) > 30) {
                    QSqlQuery query1("INSERT INTO lastlessonname ( lessonname ) VALUES ( '"+autoLessonNameBeforeEdit+"' )");
                    if (query1.lastError().isValid()) {
                        qDebug() << "DB Error: 54 - " << query1.lastError();
                    }
                    QSqlQuery query2("SELECT last_insert_rowid()");
                    if (query2.lastError().isValid()) {
                        qDebug() << "DB Error: 55 - " <<  query2.lastError();
                    }
                    query2.next();

                    QSqlQuery query;
                    query.prepare("UPDATE pupilatlesson SET stopdate = ?, llnid = ? WHERE palid = ?");
                    query.addBindValue(QDate::currentDate().toString(Qt::ISODate));
                    query.addBindValue(query2.value(0).toInt());
                    query.addBindValue(stringList.at(1).toInt());
                    query.exec();
                    if (query.lastError().isValid()) {
                        qDebug() << "DB Error: 56 - " << query.lastError();
                    }
                } else { // If no remove completly because time was to short
                    QSqlQuery query1("DELETE FROM pupilatlesson WHERE palid = "+stringList.at(1));
                    if (query1.lastError().isValid()) {
                        qDebug() << "DB Error: 57 - " << query1.lastError();
                    }
                }
            }
        }
        // 	2. see if new pupil was added
        QStringListIterator it1(tempPupilList);
        while (it1.hasNext()) {
            QString pupilId(it1.next());
            if(!pupilListBeforeEdit.contains(pupilId)) {
                QSqlQuery query;
                query.prepare("INSERT INTO pupilatlesson (lessonid, pupilid, startdate, stopdate) VALUES ( ?, ?, ?, ?)");
                // 				qDebug() << currentLessonId << pupilId;
                query.addBindValue(currentLessonId);
                query.addBindValue(pupilId.toInt());
                query.addBindValue(QDate::currentDate().toString(Qt::ISODate));
                query.addBindValue(QString("9999-99-99"));
                query.exec();
                if (query.lastError().isValid()) {
                    qDebug() << "DB Error: 58 - " << query.lastError();
                }
            }
        }
    }

    if(saveOk) {
        myW->treeWidget_timeTable->refreshTimeTable();
    }
}

void LessonTabWidget::addPupilToLesson()
{

    if(!myW->treeWidget_availablePupil->selectedItems().empty()) {
        QTreeWidgetItem *availablePupilItem = myW->treeWidget_availablePupil->takeTopLevelItem(myW->treeWidget_availablePupil->indexOfTopLevelItem(myW->treeWidget_availablePupil->selectedItems().first()));
        myW->treeWidget_pupilAtLesson->addTopLevelItem(availablePupilItem);
        myW->treeWidget_pupilAtLesson->sortByColumn (0, Qt::AscendingOrder);

        tempPupilList << availablePupilItem->data(0, Qt::UserRole).toString();
        // 		qDebug() << tempPupilList.join(", ");
    }

    updateAutoLessonName();
}

void LessonTabWidget::delPupilFromLesson()
{


    if(!myW->treeWidget_pupilAtLesson->selectedItems().empty()) {
        QTreeWidgetItem *pupilAtLessonItem = myW->treeWidget_pupilAtLesson->takeTopLevelItem(myW->treeWidget_pupilAtLesson->indexOfTopLevelItem(myW->treeWidget_pupilAtLesson->selectedItems().first()));
        myW->treeWidget_availablePupil->addTopLevelItem(pupilAtLessonItem);
        myW->treeWidget_availablePupil->sortByColumn (0, Qt::AscendingOrder);

        tempPupilList.removeAt(tempPupilList.indexOf(pupilAtLessonItem->data(0, Qt::UserRole).toString()));
        // 		qDebug() << tempPupilList.join(", ");
    }

    updateAutoLessonName();
}


void LessonTabWidget::setSingleLesson()
{
    updateAutoLessonName();
}
void LessonTabWidget::setGroupLesson()
{
    updateAutoLessonName();
}
void LessonTabWidget::setEnsembleLesson()
{
    updateAutoLessonName();
}
void LessonTabWidget::setSteadyLessonDate()
{
    updateAutoLessonName();
}
void LessonTabWidget::setUnsteadyLessonDate()
{
    updateAutoLessonName();
}

QString LessonTabWidget::updateAutoLessonName()
{
    //set autonaming string
    QString autoNameString;

    if(myW->radioButton_singleLesson->isChecked()) autoNameString = "EU-";
    else if(myW->radioButton_groupLesson->isChecked()) autoNameString = "GU-";
    else autoNameString = "EnsU-";

    if(myW->radioButton_steadyLessonDate->isChecked())
        autoNameString += QString::number(myW->timeEdit_lessonStartTime->time().secsTo(myW->timeEdit_lessonStopTime->time())/60,10)+"-";


    if(myW->comboBox_lessonLocation->currentText() != "")
        autoNameString += myW->comboBox_lessonLocation->currentText().mid(0,3)+"-";

    if(myW->treeWidget_pupilAtLesson->topLevelItemCount()) {

        if(myW->radioButton_singleLesson->isChecked()) {
            QTreeWidgetItem *item = myW->treeWidget_pupilAtLesson->topLevelItem(0);
            autoNameString += item->data(0, Qt::DisplayRole).toString().section(", ",1,1).mid(0,3)+item->data(0, Qt::DisplayRole).toString().section(", ",0,0).mid(0,3);;
        } else {
            int i;
            for (i=0; i<myW->treeWidget_pupilAtLesson->topLevelItemCount(); i++) {

                QTreeWidgetItem *item = myW->treeWidget_pupilAtLesson->topLevelItem(i);
                autoNameString += item->data(0, Qt::DisplayRole).toString().section(", ",1,1).mid(0,1);
            }
        }
    }
    myW->label_autoLessonName->setText(autoNameString);
    return autoNameString;
}

void LessonTabWidget::lessonStopTimeChanged(QTime time)
{
    // 	qDebug() << myW->timeEdit_lessonStopTime->time().secsTo(time);
    if(time.secsTo(myW->timeEdit_lessonStartTime->time()) > 0) {
        time.setHMS(time.hour(), time.minute()-1, 0 );
        myW->timeEdit_lessonStartTime->setTime(time);
    }

    calcLessonDuration();
}

void LessonTabWidget::lessonStartTimeChanged(QTime time)
{
    // 	qDebug() << myW->timeEdit_lessonStopTime->time().secsTo(time);
    if(myW->timeEdit_lessonStopTime->time().secsTo(time) > 0) {
        time.setHMS(time.hour(), time.minute()+1, 0 );
        myW->timeEdit_lessonStopTime->setTime(time);
    }

    calcLessonDuration();
}

void LessonTabWidget::calcLessonDuration()
{
    myW->spinBox_lessonDuration->setValue(myW->timeEdit_lessonStartTime->time().secsTo(myW->timeEdit_lessonStopTime->time())/60);
}

void LessonTabWidget::lessonDurationChanged()
{
    updateAutoLessonName();
}

void LessonTabWidget::lessonLocationChanged()
{
    updateAutoLessonName();
}
