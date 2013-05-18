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
#include "settingsdialogimpl.h"

#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "configfile.h"
#include "mainwindowimpl.h"

SettingsDialogImpl::SettingsDialogImpl(ConfigFile *c, mainWindowImpl *w)
    : myConfig(c), myW(w)
{
    setupUi(this);

    loadSettings();

    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( saveSettings() ) );
    connect( pushButton_getMsgSoundFile, SIGNAL( clicked() ), this, SLOT( getMsgSoundFile() ) );
    connect( pushButton_getRemSoundFile, SIGNAL( clicked() ), this, SLOT( getRemSoundFile() ) );
    connect( pushButton_changeTimeTableDayTColor, SIGNAL( clicked() ), this, SLOT( changeTimeTableDayTColor() ) );
    connect( pushButton_changeTimeTableDayBColor, SIGNAL( clicked() ), this, SLOT( changeTimeTableDayBColor() ) );
    connect( pushButton_changeTimeTableLessonTColor, SIGNAL( clicked() ), this, SLOT( changeTimeTableLessonTColor() ) );
    connect( pushButton_changeTimeTableLessonBColor, SIGNAL( clicked() ), this, SLOT( changeTimeTableLessonBColor() ) );
    connect( pushButton_changeTimeTablePupilTColor, SIGNAL( clicked() ), this, SLOT( changeTimeTablePupilTColor() ) );
    connect( pushButton_changeTimeTablePupilBColor, SIGNAL( clicked() ), this, SLOT( changeTimeTablePupilBColor() ) );
    connect( pushButton_addLessonLocation, SIGNAL( clicked() ), this, SLOT( addLessonLocation() ) );
    connect( pushButton_delLessonLocation, SIGNAL( clicked() ), this, SLOT( delLessonLocation() ) );
    connect( pushButton_addInstrument, SIGNAL( clicked() ), this, SLOT( addInstrument() ) );
    connect( pushButton_delInstrument, SIGNAL( clicked() ), this, SLOT( delInstrument() ) );
    connect( pushButton_addInstrumentSize, SIGNAL( clicked() ), this, SLOT( addInstrumentSize() ) );
    connect( pushButton_delInstrumentSize, SIGNAL( clicked() ), this, SLOT( delInstrumentSize() ) );
    connect( pushButton_addPiecesGenre, SIGNAL( clicked() ), this, SLOT( addPiecesGenre() ) );
    connect( pushButton_delPiecesGenre, SIGNAL( clicked() ), this, SLOT( delPiecesGenre() ) );
    connect( listWidget_piecesGenre, SIGNAL( currentItemChanged( QListWidgetItem *, QListWidgetItem *) ), this, SLOT( piecesGenreItemChanged( QListWidgetItem *, QListWidgetItem * ) ) );
    connect( listWidget_instruments, SIGNAL( currentItemChanged( QListWidgetItem *, QListWidgetItem *) ), this, SLOT( instrumentsItemChanged( QListWidgetItem *, QListWidgetItem * ) ) );
    connect( listWidget_instrumentSizes, SIGNAL( currentItemChanged( QListWidgetItem *, QListWidgetItem *) ), this, SLOT( instrumentSizesItemChanged( QListWidgetItem *, QListWidgetItem * ) ) );
    connect( listWidget_lessonLocations, SIGNAL( currentItemChanged( QListWidgetItem *, QListWidgetItem *) ), this, SLOT( lessonLocationsItemChanged( QListWidgetItem *, QListWidgetItem * ) ) );

}

SettingsDialogImpl::~SettingsDialogImpl() {}

void SettingsDialogImpl::loadSettings()
{
    checkBox_birthdayReminder->setChecked(myConfig->readConfigInt("BirthdayReminder"));
    groupBox_lessonEndMsg->setChecked(myConfig->readConfigInt("LessonEndMsg"));
    spinBox_minutesToLessonEndForMsg->setValue(myConfig->readConfigInt("MinutesToLessonEndForMsg"));
    lineEdit_msgSoundFilePath->setText(QString::fromUtf8(myConfig->readConfigString("MsgSoundFilePath").c_str()));
    horizontalSlider_lessonEndMsgVolume->setValue(myConfig->readConfigInt("LessonEndMsgSoundVolume"));
    lineEdit_remSoundFilePath->setText(QString::fromUtf8(myConfig->readConfigString("RemSoundFilePath").c_str()));
    horizontalSlider_remVolume->setValue(myConfig->readConfigInt("RemSoundVolume"));
    checkBox_saveNotesPiecesForAllPupil->setChecked(myConfig->readConfigInt("SaveNotesPiecesForAllPupil"));

    if(myConfig->readConfigInt("RecitalIntervalCheckerOnlySolo")) {
        radioButton_recitalIntervallCheckerSoloOnly->setChecked(TRUE);
    } else {
        radioButton_recitalIntervallCheckerSoloAndEns->setChecked(TRUE);
    }

    QPalette timeTableDayPalette;
    timeTableDayPalette.setColor(QPalette::WindowText, QColor(QString(myConfig->readConfigString("TimeTableDayTColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTableDayTColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTableDayTColor").c_str()).section(",",2,2).toInt()));
    timeTableDayPalette.setColor(QPalette::Window, QColor(QString(myConfig->readConfigString("TimeTableDayBColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTableDayBColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTableDayBColor").c_str()).section(",",2,2).toInt()));
    label_timeTableDayColor->setPalette(timeTableDayPalette);
    label_timeTableDayColor->setAutoFillBackground(TRUE);

    QPalette timeTableLessonPalette;
    timeTableLessonPalette.setColor(QPalette::WindowText, QColor(QString(myConfig->readConfigString("TimeTableLessonTColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTableLessonTColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTableLessonTColor").c_str()).section(",",2,2).toInt()));
    timeTableLessonPalette.setColor(QPalette::Window, QColor(QString(myConfig->readConfigString("TimeTableLessonBColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTableLessonBColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTableLessonBColor").c_str()).section(",",2,2).toInt()));
    label_timeTableLessonColor->setPalette(timeTableLessonPalette);
    label_timeTableLessonColor->setAutoFillBackground(TRUE);

    QPalette timeTablePupilPalette;
    timeTablePupilPalette.setColor(QPalette::WindowText, QColor(QString(myConfig->readConfigString("TimeTablePupilTColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTablePupilTColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTablePupilTColor").c_str()).section(",",2,2).toInt()));
    timeTablePupilPalette.setColor(QPalette::Window, QColor(QString(myConfig->readConfigString("TimeTablePupilBColor").c_str()).section(",",0,0).toInt(), QString(myConfig->readConfigString("TimeTablePupilBColor").c_str()).section(",",1,1).toInt(), QString(myConfig->readConfigString("TimeTablePupilBColor").c_str()).section(",",2,2).toInt()));
    label_timeTablePupilColor->setPalette(timeTablePupilPalette);
    label_timeTablePupilColor->setAutoFillBackground(TRUE);

    listWidget_piecesGenre->clear();
    listWidget_lessonLocations->clear();
    listWidget_instruments->clear();
    listWidget_instrumentSizes->clear();

    std::list<std::string> genreList = myConfig->readConfigStringList("PalPiecesGenreList");
    std::list<std::string>::iterator it1;
    for(it1= genreList.begin(); it1 != genreList.end(); it1++) {
        QListWidgetItem *item = new QListWidgetItem(QString::fromUtf8(it1->c_str()), listWidget_piecesGenre);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }

    std::list<std::string> locationList = myConfig->readConfigStringList("LessonLocationList");
    std::list<std::string>::iterator it2;
    for(it2= locationList.begin(); it2 != locationList.end(); it2++) {
        QListWidgetItem *item = new QListWidgetItem(QString::fromUtf8(it2->c_str()), listWidget_lessonLocations);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }

    std::list<std::string> instrumentList = myConfig->readConfigStringList("PupilInstrumentList");
    std::list<std::string>::iterator it3;
    for(it3= instrumentList.begin(); it3 != instrumentList.end(); it3++) {
        QListWidgetItem *item = new QListWidgetItem(QString::fromUtf8(it3->c_str()), listWidget_instruments);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }

    std::list<std::string> instrumentSizeList = myConfig->readConfigStringList("PupilInstrumentSizeList");
    std::list<std::string>::iterator it4;
    for(it4= instrumentSizeList.begin(); it4 != instrumentSizeList.end(); it4++) {
        QListWidgetItem *item = new QListWidgetItem(QString::fromUtf8(it4->c_str()), listWidget_instrumentSizes);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }

    spinBox_moderationDuration->setValue(myConfig->readConfigInt("RecitalModerationDuration"));
    spinBox_durationBetweenPieces->setValue(myConfig->readConfigInt("RecitalBetweenPiecesDuration"));

}

void SettingsDialogImpl::saveSettings()
{
    myConfig->writeConfigInt("BirthdayReminder", checkBox_birthdayReminder->isChecked());
    myConfig->writeConfigInt("LessonEndMsg", groupBox_lessonEndMsg->isChecked());
    myConfig->writeConfigInt("MinutesToLessonEndForMsg", spinBox_minutesToLessonEndForMsg->value());
    myConfig->writeConfigString("MsgSoundFilePath", lineEdit_msgSoundFilePath->text().toUtf8().constData());
    myConfig->writeConfigInt("LessonEndMsgSoundVolume", horizontalSlider_lessonEndMsgVolume->value());
    myConfig->writeConfigString("RemSoundFilePath", lineEdit_remSoundFilePath->text().toUtf8().constData());
    myConfig->writeConfigInt("RemSoundVolume", horizontalSlider_remVolume->value());
    myConfig->writeConfigInt("SaveNotesPiecesForAllPupil", checkBox_saveNotesPiecesForAllPupil->isChecked());
    myConfig->writeConfigInt("RecitalIntervalCheckerOnlySolo", radioButton_recitalIntervallCheckerSoloOnly->isChecked());

    QPalette timeTableDayPalette = label_timeTableDayColor->palette();
    myConfig->writeConfigString("TimeTableDayTColor", QString(QString::number(timeTableDayPalette.color(QPalette::WindowText).red())+","+QString::number(timeTableDayPalette.color(QPalette::WindowText).green())+","+QString::number(timeTableDayPalette.color(QPalette::WindowText).blue())).toStdString());
    myConfig->writeConfigString("TimeTableDayBColor", QString(QString::number(timeTableDayPalette.color(QPalette::Window).red())+","+QString::number(timeTableDayPalette.color(QPalette::Window).green())+","+QString::number(timeTableDayPalette.color(QPalette::Window).blue())).toStdString());

    QPalette timeTableLessonPalette = label_timeTableLessonColor->palette();
    myConfig->writeConfigString("TimeTableLessonTColor", QString(QString::number(timeTableLessonPalette.color(QPalette::WindowText).red())+","+QString::number(timeTableLessonPalette.color(QPalette::WindowText).green())+","+QString::number(timeTableLessonPalette.color(QPalette::WindowText).blue())).toStdString());
    myConfig->writeConfigString("TimeTableLessonBColor", QString(QString::number(timeTableLessonPalette.color(QPalette::Window).red())+","+QString::number(timeTableLessonPalette.color(QPalette::Window).green())+","+QString::number(timeTableLessonPalette.color(QPalette::Window).blue())).toStdString());

    QPalette timeTablePupilPalette = label_timeTablePupilColor->palette();
    myConfig->writeConfigString("TimeTablePupilTColor", QString(QString::number(timeTablePupilPalette.color(QPalette::WindowText).red())+","+QString::number(timeTablePupilPalette.color(QPalette::WindowText).green())+","+QString::number(timeTablePupilPalette.color(QPalette::WindowText).blue())).toStdString());
    myConfig->writeConfigString("TimeTablePupilBColor", QString(QString::number(timeTablePupilPalette.color(QPalette::Window).red())+","+QString::number(timeTablePupilPalette.color(QPalette::Window).green())+","+QString::number(timeTablePupilPalette.color(QPalette::Window).blue())).toStdString());

    int i;
    std::list<std::string> genreList;
    for(i=0; i<listWidget_piecesGenre->count(); i++) {
        genreList.push_back(listWidget_piecesGenre->item(i)->data(Qt::DisplayRole).toString().toStdString());
    }
    myConfig->writeConfigStringList("PalPiecesGenreList", genreList);

    std::list<std::string> locationList;
    for(i=0; i<listWidget_lessonLocations->count(); i++) {
        locationList.push_back(listWidget_lessonLocations->item(i)->data(Qt::DisplayRole).toString().toStdString());
    }
    myConfig->writeConfigStringList("LessonLocationList", locationList);

    std::list<std::string> instrumentList;
    for(i=0; i<listWidget_instruments->count(); i++) {
        instrumentList.push_back(listWidget_instruments->item(i)->data(Qt::DisplayRole).toString().toStdString());
    }
    myConfig->writeConfigStringList("PupilInstrumentList", instrumentList);

    std::list<std::string> instrumentSizeList;
    for(i=0; i<listWidget_instrumentSizes->count(); i++) {
        instrumentSizeList.push_back(listWidget_instrumentSizes->item(i)->data(Qt::DisplayRole).toString().toStdString());
    }
    myConfig->writeConfigStringList("PupilInstrumentSizeList", instrumentSizeList);

    myConfig->writeConfigInt("RecitalModerationDuration", spinBox_moderationDuration->value());
    myConfig->writeConfigInt("RecitalBetweenPiecesDuration", spinBox_durationBetweenPieces->value());

    myConfig->writeBuffer();
}

void SettingsDialogImpl::getMsgSoundFile()
{
    QString	fileName = QFileDialog::getOpenFileName(this,
                       QString::fromUtf8(tr("Klangdatei auswählen").toStdString().c_str()), QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str()), tr("Klangdateien (*.wav *.ogg *.mp3)"));
    lineEdit_msgSoundFilePath->setText(fileName);

}

void SettingsDialogImpl::getRemSoundFile()
{
    QString	fileName = QFileDialog::getOpenFileName(this,
                       QString::fromUtf8(tr("Klangdatei auswählen").toStdString().c_str()), QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str()), tr("Klangdateien (*.wav *.ogg *.mp3)"));
    lineEdit_remSoundFilePath->setText(fileName);

}

void SettingsDialogImpl::changeTimeTableDayTColor()
{
    QPalette timeTableDayPalette = label_timeTableDayColor->palette();
    QColor color = QColorDialog::getColor(timeTableDayPalette.color(QPalette::WindowText), this);
    if (color.isValid()) {
        timeTableDayPalette.setColor(QPalette::WindowText, color)	;
        label_timeTableDayColor->setPalette(timeTableDayPalette);
    }
}

void SettingsDialogImpl::changeTimeTableDayBColor()
{
    QPalette timeTableDayPalette = label_timeTableDayColor->palette();
    QColor color = QColorDialog::getColor(timeTableDayPalette.color(QPalette::Window), this);
    if (color.isValid()) {
        timeTableDayPalette.setColor(QPalette::Window, color)	;
        label_timeTableDayColor->setPalette(timeTableDayPalette);
    }
}

void SettingsDialogImpl::changeTimeTableLessonTColor()
{
    QPalette timeTableLessonPalette = label_timeTableLessonColor->palette();
    QColor color = QColorDialog::getColor(timeTableLessonPalette.color(QPalette::WindowText), this);
    if (color.isValid()) {
        timeTableLessonPalette.setColor(QPalette::WindowText, color)	;
        label_timeTableLessonColor->setPalette(timeTableLessonPalette);
    }
}

void SettingsDialogImpl::changeTimeTableLessonBColor()
{
    QPalette timeTableLessonPalette = label_timeTableLessonColor->palette();
    QColor color = QColorDialog::getColor(timeTableLessonPalette.color(QPalette::Window), this);
    if (color.isValid()) {
        timeTableLessonPalette.setColor(QPalette::Window, color)	;
        label_timeTableLessonColor->setPalette(timeTableLessonPalette);
    }
}

void SettingsDialogImpl::changeTimeTablePupilTColor()
{
    QPalette timeTablePupilPalette = label_timeTablePupilColor->palette();
    QColor color = QColorDialog::getColor(timeTablePupilPalette.color(QPalette::WindowText), this);
    if (color.isValid()) {
        timeTablePupilPalette.setColor(QPalette::WindowText, color)	;
        label_timeTablePupilColor->setPalette(timeTablePupilPalette);
    }
}

void SettingsDialogImpl::changeTimeTablePupilBColor()
{
    QPalette timeTablePupilPalette = label_timeTablePupilColor->palette();
    QColor color = QColorDialog::getColor(timeTablePupilPalette.color(QPalette::Window), this);
    if (color.isValid()) {
        timeTablePupilPalette.setColor(QPalette::Window, color)	;
        label_timeTablePupilColor->setPalette(timeTablePupilPalette);
    }
}

void SettingsDialogImpl::addLessonLocation()
{
    if(lineEdit_newLessonLocation->text() != "")
        listWidget_lessonLocations->addItem ( lineEdit_newLessonLocation->text() );
}

void SettingsDialogImpl::delLessonLocation()
{
    listWidget_lessonLocations->takeItem ( listWidget_lessonLocations->currentRow() );
}

void SettingsDialogImpl::addInstrument()
{
    if(lineEdit_newInstrument->text() != "")
        listWidget_instruments->addItem ( lineEdit_newInstrument->text() );
}

void SettingsDialogImpl::delInstrument()
{
    listWidget_instruments->takeItem ( listWidget_instruments->currentRow() );
}

void SettingsDialogImpl::addInstrumentSize()
{
    if(lineEdit_newInstrumentSize->text() != "")
        listWidget_instrumentSizes->addItem ( lineEdit_newInstrumentSize->text() );
}

void SettingsDialogImpl::delInstrumentSize()
{
    listWidget_instrumentSizes->takeItem ( listWidget_instrumentSizes->currentRow() );
}

void SettingsDialogImpl::addPiecesGenre()
{
    if(lineEdit_newPiecesGenre->text() != "")
        listWidget_piecesGenre->addItem ( lineEdit_newPiecesGenre->text() );
}

void SettingsDialogImpl::delPiecesGenre()
{
    listWidget_piecesGenre->takeItem ( listWidget_piecesGenre->currentRow() );
}

void SettingsDialogImpl::piecesGenreItemChanged(QListWidgetItem * current, QListWidgetItem * /*previous*/)
{

    if(listWidget_piecesGenre->count() > 1)
        lineEdit_newPiecesGenre->setText(current->data(Qt::DisplayRole).toString());
}

void SettingsDialogImpl::instrumentsItemChanged(QListWidgetItem * current, QListWidgetItem * /*previous*/)
{

    if(listWidget_instruments->count() > 1)
        lineEdit_newInstrument->setText(current->data(Qt::DisplayRole).toString());
}

void SettingsDialogImpl::instrumentSizesItemChanged(QListWidgetItem * current, QListWidgetItem * /*previous*/)
{

    if(listWidget_instrumentSizes->count() > 1)
        lineEdit_newInstrumentSize->setText(current->data(Qt::DisplayRole).toString());
}

void SettingsDialogImpl::lessonLocationsItemChanged(QListWidgetItem * current, QListWidgetItem * /*previous*/)
{

    if(listWidget_lessonLocations->count() > 1)
        lineEdit_newLessonLocation->setText(current->data(Qt::DisplayRole).toString());
}

