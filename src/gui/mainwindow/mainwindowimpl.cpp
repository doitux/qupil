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
#include "mainwindowimpl.h"

#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "qupil_defs.h"
#include "configfile.h"
#include "mydbhandler.h"
#include "sdlplayer.h"
#include "metronomplayer.h"

#include "settingsdialogimpl.h"
#include "concertmanagerdialogimpl.h"
#include "instrumentmanagerdialogimpl.h"
#include "birthdaysdialogimpl.h"
#include "metronomdialogimpl.h"
#include "sheetmusiclibrarydialogimpl.h"
#include "aboutqupildialogimpl.h"
#include "csvimportfieldsdialogimpl.h"
#include "buildtimetabledoc.h"
#include "pupilslastrecital/pupilslastrecitalviewdialog.h"
#include "pupilsnotinensemble/pupilsnotinensembledialog.h"
#include "reminderdialog.h"
#include "builddayviewdoc/builddayviewdialog.h"
#include "mymessagedialog/mymessagedialogimpl.h"

mainWindowImpl::mainWindowImpl(ConfigFile *c, myDBHandler *d)
    : myConfig(c), myDb(d), thisReminder1TimeAlreadyDone(""), thisReminder2TimeAlreadyDone(""), reminderAtLessonStartCounter(0)
{
    setupUi(this);

    this->setWindowTitle(QString::fromUtf8(tr("Qupil %1 - Unterrichtsmanagement für Instrumentalpädagogen - qupil.de").arg(RELEASE_STRING).toStdString().c_str()));

    this->installEventFilter(this);

    statusBarStats = new QLabel(QString::fromUtf8("Unterrichtseinheiten: 0 - Schüler: 0 ").toStdString().c_str());
    statusBar()->addWidget(statusBarStats);
    statusBarSpace = new QLabel("");
    statusBar()->addWidget(statusBarSpace, 1);
    statusBarLehrsaiten = new QLabel("<a href='http://qupil.de' target='_blank'> qupil.de</a>");
    statusBarLehrsaiten->setOpenExternalLinks(true);
    statusBar()->addWidget(statusBarLehrsaiten);

    tabWidget_lesson->setMyW(this);
    tabWidget_lesson->setMyConfig(myConfig);
    tabWidget_pupil->setMyW(this);
    tabWidget_pupil->setMyConfig(myConfig);
    treeWidget_timeTable->setMyW(this);
    treeWidget_timeTable->setMyConfig(myConfig);
    treeWidget_pupilList->setMyW(this);
    treeWidget_pupilList->setMyConfig(myConfig);
    treeWidget_pupilArchiveList->setMyConfig(myConfig);
    treeWidget_pupilArchiveList->setMyW(this);
    textBrowser_pupilArchive->setMyConfig(myConfig);
    textBrowser_pupilArchive->setMyW(this);

    tabWidget_lesson->init();
    tabWidget_pupil->init();
    textBrowser_pupilArchive->init();

    switch (comboBox_leftListMode->currentIndex()) {
    case 0: {
        treeWidget_timeTable->refreshTimeTable();
        treeWidget_timeTable->selectFirstTodayLessonItem();
    }
    break;
    case 1: {
        treeWidget_pupilList->refreshPupilList();
        treeWidget_pupilList->selectFirstItem();
    }
    break;
    }

    // 	Sound
    mySDLPlayer = new SDLPlayer(myConfig);
    myMetronomPlayer = new MetronomPlayer(mySDLPlayer);
    if(!myMetronomPlayer->loadSounds(QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str())+"sounds/s2.wav", QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str())+"sounds/s2.wav",QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str())+"sounds/s3.wav")) {
        qDebug() << "Konnte Metronom Sounds nicht laden";
    }
    mySDLPlayer->setMetronomPlayer(myMetronomPlayer);

    //Dialogs
    mySettingsDialog = new SettingsDialogImpl(myConfig, this);
    myConcertManagerDialog = new ConcertManagerDialogImpl(myConfig, this);
    myInstrumentManagerDialog = new InstrumentManagerDialogImpl(myConfig, this);
    myBirthdaysDialog = new BirthdaysDialogImpl(myConfig, this);
    myMetronomDialog = new MetronomDialogImpl(myConfig, this, mySDLPlayer, myMetronomPlayer);
    mySheetMusicLibraryDialog = new SheetMusicLibraryDialogImpl(myConfig, this);
    myAboutQupilDialog = new AboutQupilDialogImpl(this, myConfig);
    myCsvImportFieldsDialog = new CsvImportFieldsDialogImpl(myConfig, this);

    myBuildTimeTableDoc = new BuildTimeTableDoc(myConfig);

    mySDLPlayer->setMyMetronomDialog(myMetronomDialog);

    actionUnterrichtLoeschen->setVisible(true);
    actionSchuelerLoeschen->setVisible(false);
    actionSchuelerArchivierenUndLoeschen->setVisible(false);
    actionSchuelerArchivieren->setVisible(false);
    actionArchiveintragLoeschen->setVisible(false);

    lessonEndMsgTimer = new QTimer(this);
    lessonEndMsgTimer->start(10000);

    reminderTimer = new QTimer(this);
    QTimer *singShot1 = new QTimer(this);
    QTimer *singShot2 = new QTimer(this);
    QTimer *singShot3 = new QTimer(this);

    connect(lessonEndMsgTimer, SIGNAL(timeout()), this, SLOT(lessonEndMsgPlayer()));
    connect(reminderTimer, SIGNAL(timeout()), this, SLOT(showReminder()));
    connect(singShot1, SIGNAL(timeout()), this, SLOT(checkForProgramStartReminder()));
    connect(singShot3, SIGNAL(timeout()), this, SLOT(reminderAtStartupHack()));
    connect( actionUeberQupil, SIGNAL( triggered() ), this, SLOT( callAboutQupilDialog() ) );
    connect( actionQupil_einrichten, SIGNAL( triggered() ), this, SLOT( callSettingsDialog() ) );
    connect( actionMetronomStimmen, SIGNAL( triggered() ), this, SLOT( callMetronomDialog() ) );
    connect( actionVorspielManager, SIGNAL( triggered() ), this, SLOT( callConcertManagerDialog() ) );
    connect( actionInstrumentenManager, SIGNAL( triggered() ), this, SLOT( callInstrumentManagerDialog() ) );
    connect( actionGeburtstageAnzeigen, SIGNAL( triggered() ), this, SLOT( callBirthdaysDialog() ) );
    connect( actionNotenBibliothek, SIGNAL( triggered() ), this, SLOT( callSheetMusicLibraryDialog() ) );
    connect( actionNeuerSchueler, SIGNAL( triggered() ), this, SLOT( addNewPupil() ) );
    connect( actionNeuerUnterricht, SIGNAL( triggered() ), this, SLOT( addNewLesson() ) );
    connect( actionVorspielkandidaten, SIGNAL( triggered() ), this, SLOT( checkForRecitalPupils() ) );
    connect( actionEnsemblekandidaten, SIGNAL( triggered() ), this, SLOT( checkForEnsemblePupils() ) );
    connect( actionUnterrichtLoeschen, SIGNAL( triggered() ), treeWidget_timeTable, SLOT( delCurrentLesson() ) );
    connect( actionSchuelerLoeschen, SIGNAL( triggered() ), treeWidget_pupilList, SLOT( delCurrentPupil() ) );
    connect( actionSchuelerArchivierenUndLoeschen, SIGNAL( triggered() ), treeWidget_pupilList, SLOT( archiveAndDelCurrentPupil() ) );
    connect( actionSchuelerArchivieren, SIGNAL( triggered() ), treeWidget_pupilList, SLOT( archiveCurrentPupil() ) );
    connect( actionArchiveintragLoeschen, SIGNAL( triggered() ), treeWidget_pupilArchiveList, SLOT( delCurrentArchive() ) );
    connect( actionBackupErstellen, SIGNAL( triggered() ), this, SLOT( createBackup() ) );
    connect( actionBackupZurueckspielen, SIGNAL( triggered() ), this, SLOT( restoreBackup() ) );
    connect( actionAdressbuchCsvImportieren, SIGNAL( triggered() ), this, SLOT( addNewPupilsFromCsv() ) );
    connect( actionStundenplanErstellen, SIGNAL( triggered() ), this, SLOT( showTimeTableDoc() ) );
    connect( actionErinnerungen, SIGNAL( triggered() ), this, SLOT( callReminderDialog() ) );
    connect( actionTagesuebersicht, SIGNAL( triggered() ), this, SLOT( callBuildDayViewDialog() ) );
    connect( comboBox_leftListMode, SIGNAL ( currentIndexChanged( int ) ), this, SLOT ( leftListModeChanged( int ) ) );
    connect( stackedWidget_leftList, SIGNAL ( currentChanged( int ) ), this, SLOT ( rightTabsChanged(int) ) );

    if(myConfig->readConfigInt("BirthdayReminder")) {
        connect(singShot2, SIGNAL(timeout()), this, SLOT(birthdayReminder()));
        singShot2->setSingleShot(true);
        singShot2->start(2000);
    }
    //refresh menu
    rightTabsChanged(0);

    //DB date time now for debugging
    qDebug() << QDateTime::currentDateTime();

    restoreWindowGeometry();

    //check for Programmstart Reminder
    singShot1->setSingleShot(true);
    singShot1->start(1800);

    //check for normal Reminder directly after Program start
    singShot3->setSingleShot(true);
    singShot3->start(1600);


    checkIfReminderTimerNeeded();

}

mainWindowImpl::~mainWindowImpl() {}

void mainWindowImpl::checkForOldDataStructure()
{

    //check for old content structure and start convert to database
    QDir oldContentDir(QString::fromUtf8(myConfig->readConfigString("UserDir").c_str())+"/content");
    if(oldContentDir.exists() && myConfig->readConfigInt("CurrentDataStructureRevision") == 0 && myConfig->readConfigInt("ThisVersionDataStructureRevision") >= 1) {

        QMessageBox::information(this, tr("Qupil"),
                                 tr("Diese Version von Qupil verwendet ein neues Format um Daten zu speichern. \nQupil wird nun versuchen die alten Daten in das neue Format umzuwandeln.\nDieser Vorgang kann einige Minuten dauern."), QMessageBox::Ok);

        //count pupils for progress dialog
        QDir oldContentDir(QString::fromUtf8(myConfig->readConfigString("UserDir").c_str())+"/content");
        QStringList subdirsTemp;
        subdirsTemp.clear();
        subdirsTemp = oldContentDir.entryList();
        subdirsTemp.removeAt(subdirsTemp.indexOf("."));
        subdirsTemp.removeAt(subdirsTemp.indexOf(".."));
        subdirsTemp.removeAt(subdirsTemp.indexOf("groups"));
        int pupilDirsInt = oldContentDir.entryList().count();

        //count groups for progress dialog
        QDir oldGroupDir(QString::fromUtf8(myConfig->readConfigString("UserDir").c_str())+"/content/groups/");
        QFile groupsFile (oldGroupDir.absolutePath()+"/groups.txt");
        int groupIdCounter = 1;
        if ( groupsFile.exists() ) {
            if (groupsFile.open( QIODevice::ReadOnly )) {
                QTextStream readStream( &groupsFile );
                while ( !readStream.atEnd() ) {
                    groupIdCounter++;
                    readStream.readLine();
                }
            }
        }

        convertOldDataProgress = new QProgressDialog(this);
        convertOldDataProgress->setWindowTitle(tr("Qupil - Alte Daten konvertieren"));
        QPushButton *cancel = new QPushButton(tr("Abbrechen"));
        convertOldDataProgress->setCancelButton ( cancel );
        cancel->hide();
        convertOldDataProgress->setMinimum(0);
        convertOldDataProgress->setMinimumWidth(350);
        convertOldDataProgress->setMaximum(groupIdCounter+pupilDirsInt);
        convertOldDataProgress->setWindowModality(Qt::WindowModal);
        convertOldDataProgress->setLabelText(tr("Konvertiere alte Daten ..."));

        if(myDb->convertObsoletePersonalData()) {
            QMessageBox::information(this, tr("Qupil"),
                                     tr("Konvertieren der alten Daten in das neue Format erfolgreich Abgeschlossen!"), QMessageBox::Ok);
        } else {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Qupil"));
            msgBox.setTextFormat(Qt::RichText);
            msgBox.setText(tr("Beim konvertieren der alten Daten ist ein Fehler aufgetreten. \nBitte wenden Sie sich an den Programm-Author: <a href=\"mailto:fhammer@qupil.de\">fhammer@qupil.de</a> um dieses Problem zu beheben!"));
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();

        }
        treeWidget_pupilList->refreshPupilList();
        treeWidget_pupilList->selectFirstItem();
        treeWidget_timeTable->refreshTimeTable();
        treeWidget_timeTable->selectFirstLessonItem();
        //refresh menu
        rightTabsChanged(0);

        myConfig->writeConfigInt("CurrentDataStructureRevision", 1);
        myConfig->writeBuffer();
    }

    //for 1.1 check for a new db schema IN CONFIG later only in DB
    if(myConfig->readConfigInt("CurrentDataStructureRevision") <= 1 && myConfig->readConfigInt("ThisVersionDataStructureRevision") == 2) {
        //update DB structure
        if(myDb->updateDBStructure(myConfig->readConfigInt("CurrentDataStructureRevision"), myConfig->readConfigInt("ThisVersionDataStructureRevision"))) {

            int temp = myConfig->readConfigInt("ThisVersionDataStructureRevision");
            myConfig->writeConfigInt("CurrentDataStructureRevision", temp);
            myConfig->writeBuffer();
        } else {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Qupil"));
            msgBox.setTextFormat(Qt::RichText);
            msgBox.setText(QString::fromUtf8(tr("Beim Update der Datenbankstruktur ist ein Fehler aufgetreten. Möglicherweise kann Qupil die Daten nicht korrekt verarbeiten. \nBitte wenden Sie sich an den Programm-Author: <a href=\"mailto:fhammer@qupil.de\">fhammer@qupil.de</a> um dieses Problem zu beheben!").toStdString().c_str()));
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }
    }
    //for later releases >1.1 dont longer use CurrentDataStructureRev from config but read from dbinfos//
    if(myDb->dBUpdateNeeded()) {
        QStringList returnValue = myDb->updateDB();
        if(returnValue.at(0) == "0") {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Qupil - Fehler"));
            msgBox.setTextFormat(Qt::RichText);
            msgBox.setText(QString::fromUtf8(tr("Beim Update der Datenbankstruktur von Revision %0 auf %1 ist folgender Fehler aufgetreten:<br><b>%2</b><br>Bitte wenden Sie sich an den Programm-Author: <a href=\"mailto:fhammer@qupil.de\">fhammer@qupil.de</a> um dieses Problem zu beheben!").toStdString().c_str()).arg(returnValue.at(1)).arg(returnValue.at(2)).arg(returnValue.at(3)));
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }
    }
}

void mainWindowImpl::updateConvertOldDataProgress(int value, QString text)
{

    if(value != -1) {
        convertOldDataProgress->setWindowTitle(tr("Qupil - Alte Daten konvertieren"));
        convertOldDataProgress->setLabelText(text);
        convertOldDataProgress->setValue(value);
    } else {
        convertOldDataProgress->cancel();
    }
}

//reports mit datum-bereich abfragen
//SELECT l.lessonname, r.date, r.content FROM lesson l, report r, pupil p, pupilatlesson pal WHERE p.surname='Röttering' AND p.forename='Michael' AND p.pupilid=pal.pupilid AND pal.lessonid=l.lessonid AND pal.lessonid=r.lessonid AND r.date >= pal.startdate AND r.date < pal.stopdate

void mainWindowImpl::callSettingsDialog()
{

    mySettingsDialog->exec();
    if (mySettingsDialog->result() == QDialog::Accepted) {
        switch (comboBox_leftListMode->currentIndex()) {
        case 0: {
            treeWidget_timeTable->refreshTimeTable();
        }
        break;
        case 1: {
            treeWidget_pupilList->refreshPupilList();
        }
        break;
        }
    }
}

void mainWindowImpl::birthdayReminder()
{
    myBirthdaysDialog->exec(false);
}

void mainWindowImpl::callBirthdaysDialog()
{
    myBirthdaysDialog->exec(true);
}

void mainWindowImpl::leftListModeChanged(int index)
{
    switch (index) {
    case 0: {
        treeWidget_timeTable->refreshTimeTable();
        QList<QTreeWidgetItem *> selectedItemList = treeWidget_timeTable->selectedItems();
        if(selectedItemList.empty()) treeWidget_timeTable->selectFirstLessonItem();
    }
    break;
    case 1: {
        treeWidget_pupilList->refreshPupilList();
        QList<QTreeWidgetItem *> selectedItemList = treeWidget_pupilList->selectedItems();
        if(selectedItemList.empty()) treeWidget_pupilList->selectFirstItem();
    }
    break;
    case 2: {
        treeWidget_pupilArchiveList->refreshPupilArchiveList();
        QList<QTreeWidgetItem *> selectedItemList = treeWidget_pupilArchiveList->selectedItems();
        if(selectedItemList.empty()) treeWidget_pupilArchiveList->selectFirstItem();
    }
    break;
    }
}

void mainWindowImpl::addNewPupil()
{
    QSqlQuery query("INSERT INTO pupil ( forename, surname, birthday, firstlessondate ) VALUES ( '"+tr("neuer")+"', '"+QString::fromUtf8(tr("Schüler").toStdString().c_str())+"', '2000-01-01', '"+QDate::currentDate().toString(Qt::ISODate)+"' ); ", *myDb->getMyPupilDb() );
    if (query.lastError().isValid()) qDebug() << "DB Error: 59 - "  << query.lastError();
    QSqlQuery query1("SELECT last_insert_rowid()", *myDb->getMyPupilDb() );
    if (query1.lastError().isValid()) qDebug() << "DB Error: 60 - " << query1.lastError();
    query1.next();
    QString newItemIdString = query1.value(0).toString();

    if(comboBox_leftListMode->currentIndex() != 1)
        comboBox_leftListMode->setCurrentIndex(1);
    else {
        treeWidget_pupilList->refreshPupilList();
        //refresh menu
        rightTabsChanged(1);
    }

    tabWidget_pupil->setCurrentIndex(2);

    if(treeWidget_pupilList->topLevelItemCount()) {
        // 	find new Pupil to select
        int i;
        for (i=0; i<treeWidget_pupilList->topLevelItemCount(); i++) {

            QTreeWidgetItem *item = treeWidget_pupilList->topLevelItem(i);
            if(item->data(0, Qt::UserRole).toString() == newItemIdString) {
                treeWidget_pupilList->setCurrentItem(item);
                break;
            }
        }
    }

}

void mainWindowImpl::addNewLesson()
{
    QSqlQuery insertLessonQuery("INSERT INTO lesson ( state, type, autolessonname, lessonname, unsteadylesson) VALUES ( 1, 1, 1, 'EU - neu', 1)", *myDb->getMyPupilDb());
    if (insertLessonQuery.lastError().isValid()) qDebug() << "DB Error: 61 - " << insertLessonQuery.lastError();
    QSqlQuery query1("SELECT last_insert_rowid()", *myDb->getMyPupilDb() );
    if (query1.lastError().isValid()) qDebug() << "DB Error: 62 - " << query1.lastError();
    query1.next();
    QString newItemIdString = "l"+query1.value(0).toString();

    if(comboBox_leftListMode->currentIndex() != 0)
        comboBox_leftListMode->setCurrentIndex(0);
    else {
        treeWidget_timeTable->refreshTimeTable();
        //refresh menu
        rightTabsChanged(0);
    }

    if(treeWidget_timeTable->topLevelItemCount()) {
        // 	find new Pupil to select
        int i;
        for (i=0; i<treeWidget_timeTable->topLevelItemCount(); i++) {

            QTreeWidgetItem *item = treeWidget_timeTable->topLevelItem(i);
            if(item->data(0, Qt::UserRole).toString() == newItemIdString) {
                treeWidget_timeTable->setCurrentItem(item);
                break;
            }
        }
    }
}

void mainWindowImpl::lessonEndMsgPlayer()
{
    if(myConfig->readConfigInt("LessonEndMsg")) {
        QTime now = QTime::currentTime();
        QDate today = QDate::currentDate();
        QSqlQuery query("SELECT lessonstoptime FROM lesson WHERE state = 1 AND lessonday="+QString::number(today.dayOfWeek()-1)+" ORDER BY lessonstoptime ASC");
        if (query.lastError().isValid()) qDebug() << "DB Error: 63 - " << query.lastError();
        while(query.next()) {
            QTime lessonEnd = QTime::fromString(query.value(0).toString(), "hh:mm");
            if(now.secsTo(lessonEnd) < myConfig->readConfigInt("MinutesToLessonEndForMsg")*60 && now.secsTo(lessonEnd) > myConfig->readConfigInt("MinutesToLessonEndForMsg")*60-11) {
                mySDLPlayer->playSound(QString::fromUtf8(myConfig->readConfigString("MsgSoundFilePath").c_str()), myConfig->readConfigInt("LessonEndMsgSoundVolume"), true);
            }
        }
    }
}

void mainWindowImpl::rightTabsChanged(int index)
{
    switch (index) {
    case 0: {
        if(treeWidget_timeTable->topLevelItemCount()) {
            menuBearbeiten->setEnabled(true);
            actionUnterrichtLoeschen->setVisible(true);
            actionSchuelerLoeschen->setVisible(false);
            actionSchuelerArchivierenUndLoeschen->setVisible(false);
            actionSchuelerArchivieren->setVisible(false);
            actionArchiveintragLoeschen->setVisible(false);
        } else {
            menuBearbeiten->setEnabled(false);
        }
    }
    break;
    case 1: {
        if(treeWidget_pupilList->topLevelItemCount()) {
            menuBearbeiten->setEnabled(true);
            actionUnterrichtLoeschen->setVisible(false);
            actionSchuelerLoeschen->setVisible(true);
            actionSchuelerArchivierenUndLoeschen->setVisible(true);
            actionSchuelerArchivieren->setVisible(true);
            actionArchiveintragLoeschen->setVisible(false);
        } else {
            menuBearbeiten->setEnabled(false);
        }
    }
    break;
    case 2: {
        if(treeWidget_pupilArchiveList->topLevelItemCount()) {
            menuBearbeiten->setEnabled(true);
            actionUnterrichtLoeschen->setVisible(false);
            actionSchuelerLoeschen->setVisible(false);
            actionSchuelerArchivierenUndLoeschen->setVisible(false);
            actionSchuelerArchivieren->setVisible(false);
            actionArchiveintragLoeschen->setVisible(true);
        } else {
            menuBearbeiten->setEnabled(false);
        }
    }
    break;
    }
}

void mainWindowImpl::callConcertManagerDialog()
{
    myConcertManagerDialog->exec();
    tabWidget_pupil->loadPupilActivity();
    tabWidget_pupil->refreshPieces();

}

void mainWindowImpl::callInstrumentManagerDialog()
{
    myInstrumentManagerDialog->exec();
}

void mainWindowImpl::callMetronomDialog()
{
    myMetronomDialog->show();
}

void mainWindowImpl::callSheetMusicLibraryDialog()
{
    mySheetMusicLibraryDialog->exec();
    switch (comboBox_leftListMode->currentIndex()) {
    case 0: {
        treeWidget_timeTable->refreshTimeTable();
    }
    break;
    case 1: {
        treeWidget_pupilList->refreshPupilList();
    }
    break;
    }
}

void mainWindowImpl::callAboutQupilDialog()
{
    myAboutQupilDialog->exec();
}


void mainWindowImpl::createBackup()
{
    QString dir = QFileDialog::getExistingDirectory(this, QString::fromUtf8(tr("Qupil - Backupverzeichnis wählen").toStdString().c_str()),
                  QDir::homePath(),
                  QFileDialog::ShowDirsOnly
                  | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty()) {

        QFile backupfile(dir+"/qupil-backup-"+QDateTime::currentDateTime().toString("yyyy-MM-dd_hhmmss")+".db");


        QString pupilDBPath(QString::fromUtf8(myConfig->readConfigString("UserDir").c_str())+"/db/qupil.db");
        QFile pupilDbFile(pupilDBPath);

        if (pupilDbFile.exists()) {

            if (pupilDbFile.copy(backupfile.fileName())) {
                QMessageBox::information(this, tr("Qupil - Backup erstellen"),
                                         tr("Das Backup wurde mit dem Dateinamen \"%1\" erfolgreich erstellt!").arg(backupfile.fileName()), QMessageBox::Ok);
            } else {
                QMessageBox::critical(this, tr("Qupil - Backup erstellen"),
                                      QString::fromUtf8(tr("Das Backup konnte im ausgewählten Verzeichnis \"%1\"nicht erstellt werden.\nBitte überprüfen Sie ob Sie Schreibrechte für dieses Verzeichnis besitzen!").arg(dir).toStdString().c_str()), QMessageBox::Ok);
            }
        }
    }
}

void mainWindowImpl::restoreBackup()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromUtf8(tr("Qupil - Backupdatei öffnen").toStdString().c_str()),
                       QDir::homePath(),
                       tr("Qupil Backupdatei (*.db)"));

    if(!fileName.isEmpty()) {

        QFile backupfile(fileName);

        QString pupilDBPath(QString::fromUtf8(myConfig->readConfigString("UserDir").c_str())+"db/qupil.db");
        QFile pupilDbFile(pupilDBPath);

        if (backupfile.exists() && backupfile.size()) {
            int ret = QMessageBox::warning(this, QString::fromUtf8(tr("Qupil - Backup zurückspielen").toStdString().c_str()),
                                           QString::fromUtf8(tr("Ein Backup sollte nur als Datenwiederherstellung nach Datenverlust zurückgespielt werden.\nWenn Sie ein Backup zurückspielen werden Änderungen und neue Einträge,\ndie eventuell nach dem Backup geschrieben wurden wieder gelöscht.\n\nMöchten Sie das Backup wirklich zurückspielen?").toStdString().c_str()),
                                           QMessageBox::Ok | QMessageBox::Cancel);
            if(ret == QMessageBox::Ok) {

                QString pupilDbFileName = pupilDbFile.fileName();

                if (backupfile.copy(pupilDbFileName+".new")) {
                    //TODO check for db structure and set currentdatastructure to corresponding one
                    bool ok2 = false;
                    QSqlDatabase *tmpDB = new QSqlDatabase;
                    *tmpDB = QSqlDatabase::addDatabase("QSQLITE");
                    tmpDB->setDatabaseName(backupfile.fileName());

                    if(tmpDB->open()) {
                        if(tmpDB->tables().contains("dbinfos")) {
                            QSqlQuery q(*tmpDB);
                            q.exec("SELECT data_structure_rev FROM dbinfos WHERE id = 0");
                            if (q.lastError().isValid()) qDebug() << "DB Error: 181 - "  << q.lastError();
                            else {
                                q.next();
                                myConfig->writeConfigInt("CurrentDataStructureRevision", q.value(0).toInt());
                                myConfig->writeBuffer();
                                ok2 = true;
                            }
                        } else {
                            //if there is no dbinfos entry currentdatastructure is 1
                            myConfig->writeConfigInt("CurrentDataStructureRevision", 1);
                            myConfig->writeBuffer();
                            ok2 = true;
                        }
                    } else {
                        qDebug() << "Cannot open backup db to check for datastructure";
                    }

                    if(ok2) {
                        QMessageBox::information(this, QString::fromUtf8(tr("Qupil - Backup zurückspielen").toStdString().c_str()),
                                                 QString::fromUtf8(tr("Das Backup wurde erfolgreich zurückgespielt.\nBitte starten Sie Qupil neu!").toStdString().c_str()), QMessageBox::Ok);
                        this->close();
                    } else {
                        QMessageBox msgBox(this);
                        msgBox.setWindowTitle(tr("Qupil - Fehler"));
                        msgBox.setTextFormat(Qt::RichText);
                        msgBox.setText(QString::fromUtf8(tr("Der Backup-Typ konnte nicht ermittelt werden! Somit kann das Backup nicht zurückgespielt werden.\nBitte wenden Sie sich an den Programm-Author: <a href=\"mailto:fhammer@qupil.de\">fhammer@qupil.de</a> um dieses Problem zu beheben!").toStdString().c_str()));
                        msgBox.setIcon(QMessageBox::Critical);
                        msgBox.exec();
                    }
                } else {
                    QMessageBox msgBox(this);
                    msgBox.setWindowTitle(tr("Qupil - Fehler"));
                    msgBox.setTextFormat(Qt::RichText);
                    msgBox.setText(QString::fromUtf8(tr("Das Backup konnte nicht zurückgespielt werden! \nBitte wenden Sie sich an den Programm-Author: <a href=\"mailto:fhammer@qupil.de\">fhammer@qupil.de</a> um dieses Problem zu beheben!").toStdString().c_str()));
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();
                }
            }
        } else {
            QMessageBox::critical(this, tr("Qupil - Fehler"),
                                  QString::fromUtf8(tr("Die ausgewählte Backupdatei existiert nicht oder ist leer. \nDas Backup wird nicht zurückgespielt!").toStdString().c_str()), QMessageBox::Ok);
        }
    }
}

void mainWindowImpl::addNewPupilsFromCsv()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromUtf8(tr("Qupil - Adressdatei öffnen").toStdString().c_str()),
                       QDir::homePath(),
                       tr("Adressen CSV (*.csv)"));
    if(!fileName.isEmpty()) {

        myCsvImportFieldsDialog->exec(fileName);
    }
}

void mainWindowImpl::showTimeTableDoc()
{

    myBuildTimeTableDoc->run();
}

void mainWindowImpl::checkForRecitalPupils()
{
    PupilsLastRecitalViewDialog dlg(myConfig, this);
    dlg.exec();
}

void mainWindowImpl::checkForEnsemblePupils()
{
    PupilsNotInEnsembleDialog dlg(myConfig, this);
    dlg.exec();
}

void mainWindowImpl::refreshTimeTableStats(int lessons, int pupils)
{
    statusBarStats->setText(QString::fromUtf8("Unterrichtseinheiten: %1 - Schüler: %2 ").arg(lessons).arg( pupils).toStdString().c_str());

}

void mainWindowImpl::callReminderDialog()
{
    ReminderDialog *dlg = new ReminderDialog;
    dlg->exec();
    checkIfReminderTimerNeeded();
}

void mainWindowImpl::checkForProgramStartReminder()
{
    QSqlQuery query;
    query.prepare("SELECT * FROM reminder WHERE mode = 0");
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 243 - " << query.lastError();
    } else {
        while(query.next()) {
            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setModal(false);
            msgBox->setWindowTitle(tr("Qupil"));
            msgBox->setTextFormat(Qt::RichText);
            msgBox->setText(QString("<u>Erinnerung:</u><br><br><b>%1</b><br>").arg(query.value(1).toString()));
            msgBox->setIcon(QMessageBox::Information);
            msgBox->show();
            if(query.value(4).toInt()) {
                mySDLPlayer->playSound(QString::fromUtf8(myConfig->readConfigString("RemSoundFilePath").c_str()), myConfig->readConfigInt("RemSoundVolume"), true);
            }
        }
    }

}

void mainWindowImpl::checkIfReminderTimerNeeded()
{

    QSqlQuery query;
    query.prepare("SELECT * FROM reminder WHERE mode != 0");
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 245 - " << query.lastError();
    } else {
        if(query.next()) {
            reminderTimer->start(58000);
        } else {
            reminderTimer->stop();
        }
    }
}

void mainWindowImpl::showReminder(bool directlyAfterStartup)
{
    int multiplePeoplePerLessonLastLessonId = -1;
    QString now;
    if(directlyAfterStartup) {
        now = myDb->getCurrentLessonStartTime();
    } else {
        now = QTime::currentTime().toString("hh:mm");
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM reminder WHERE mode != 0");
    query.exec();
    if (query.lastError().isValid()) {
        qDebug() << "DB Error: 249 - " << query.lastError();
    } else {

        if(QTime::currentTime().toString("hh:mm") != thisReminder1TimeAlreadyDone) {
            reminderAtLessonStartCounter = 0;
        }

        while(query.next()) {

            //at first check for mode 1 --> bei jedem unterrichtsanfang
            if(query.value(2).toInt() == 1) {

                QSqlQuery query5;
                query5.prepare("SELECT count(*) FROM reminder WHERE mode = 1");
                query5.exec();
                if (query.lastError().isValid()) {
                    qDebug() << "DB Error: 249 - " << query.lastError();
                }
                query5.next();

                //                qDebug() << reminderAtLessonStartCounter << query5.value(0).toInt();
                if(now != thisReminder1TimeAlreadyDone || reminderAtLessonStartCounter < query5.value(0).toInt() ) {

                    QDate today = QDate::currentDate();

                    QSqlQuery query2;
                    query2.prepare("SELECT lessonid FROM lesson WHERE state = 1 AND lessonday= ? AND lessonstarttime = ?");
                    query2.addBindValue(QString::number(today.dayOfWeek()-1));
                    query2.addBindValue(now);
                    query2.exec();
                    if (query2.lastError().isValid()) qDebug() << "DB Error: 244 - " << query2.lastError();
                    else {
                        if(query2.next()) {

                            QMessageBox *msgBox = new QMessageBox(this);
                            msgBox->setModal(false);
                            msgBox->setWindowTitle(tr("Qupil"));
                            msgBox->setTextFormat(Qt::RichText);
                            msgBox->setText(QString("<u>Erinnerung:</u><br><br><b>%1</b><br>").arg(query.value(1).toString()));
                            msgBox->setIcon(QMessageBox::Information);
                            msgBox->show();
                            if(query.value(4).toInt()) {
                                mySDLPlayer->playSound(QString::fromUtf8(myConfig->readConfigString("RemSoundFilePath").c_str()), myConfig->readConfigInt("RemSoundVolume"), true);
                            }
                            reminderAtLessonStartCounter++;
                            thisReminder1TimeAlreadyDone = now;
                        }
                    }
                }
            }
            //at first check for mode 2 --> bei speziellem Schüler
            if(query.value(2).toInt() == 2) {

                if(now != thisReminder2TimeAlreadyDone || multiplePeoplePerLessonLastLessonId != -1 ) {

                    QDate today = QDate::currentDate();
                    QSqlQuery query2;
                    query2.prepare("SELECT lessonid FROM lesson WHERE state = 1 AND lessonday= ? AND lessonstarttime = ?");
                    query2.addBindValue(QString::number(today.dayOfWeek()-1));
                    query2.addBindValue(now);
                    query2.exec();
                    if (query2.lastError().isValid()) qDebug() << "DB Error: 246 - " << query2.lastError();
                    else {
                        if(query2.next()) {

                            QSqlQuery query3;
                            query3.prepare("SELECT palid FROM pupilatlesson WHERE lessonid = ? AND pupilid = ?");
                            query3.addBindValue(query2.value(0).toInt());
                            query3.addBindValue(query.value(3).toInt());
                            query3.exec();
                            if (query3.lastError().isValid()) qDebug() << "DB Error: 247 - " << query3.lastError();
                            else {
                                if(query3.next()) {

                                    QSqlQuery query4;
                                    query4.prepare("SELECT surname, forename FROM pupil WHERE pupilid = ?");
                                    query4.addBindValue(query.value(3).toInt());
                                    query4.exec();
                                    if (query4.lastError().isValid()) qDebug() << "DB Error: 248 - " << query4.lastError();
                                    else {
                                        if(query4.next()) {
                                            myMessageDialogImpl *msgBox = new myMessageDialogImpl(this, myDb);
                                            msgBox->setModal(false);
                                            msgBox->setWindowTitle(tr("Qupil"));
                                            msgBox->setTextFormat(Qt::RichText);
                                            msgBox->setText(QString::fromUtf8(QString("<u>Erinnerung für den Schüler ").toStdString().c_str())+QString("\"%1\":</u><br><br><b>%2</b><br>").arg(query4.value(0).toString()+", "+query4.value(1).toString()).arg(query.value(1).toString()));
                                            msgBox->setIcon(QMessageBox::Information);
                                            msgBox->show(query.value(0).toInt(), true, "Erledigt? - Die Erinnerung wird entfernt!");

                                            if(query.value(4).toInt()) {
                                                mySDLPlayer->playSound(QString::fromUtf8(myConfig->readConfigString("RemSoundFilePath").c_str()), myConfig->readConfigInt("RemSoundVolume"), true);
                                            }
                                            // diese variable soll ermöglichen dass auch mehrere schüler im selben
                                            // unterricht unterschiedliche perönliche Reminder angezeigt bekommen
                                            multiplePeoplePerLessonLastLessonId = query2.value(0).toInt();

                                            thisReminder2TimeAlreadyDone = now;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool mainWindowImpl::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Close) {
        saveWindowGeometry();
        return QMainWindow::eventFilter(obj, event);
    } else {
        // pass the event on to the parent class
        return QMainWindow::eventFilter(obj, event);
    }
}

void mainWindowImpl::saveWindowGeometry()
{
    if(this->isFullScreen()) {
        myConfig->writeConfigInt("WindowFullScreenSave", 1);
    } else {
        myConfig->writeConfigInt("WindowFullScreenSave", 0);
        myConfig->writeConfigInt("WindowHeightSave", this->height());
        myConfig->writeConfigInt("WindowWidthSave", this->width());
    }
    myConfig->writeBuffer();
}

void mainWindowImpl::restoreWindowGeometry()
{
    if(myConfig->readConfigInt("WindowFullScreenSave")) {
        this->showFullScreen();
    } else {
        this->resize(myConfig->readConfigInt("WindowWidthSave"), myConfig->readConfigInt("WindowHeightSave"));
    }
}

void mainWindowImpl::callBuildDayViewDialog()
{
    BuildDayViewDialog dlg(myConfig, this);
    if(dlg.exec() == QDialog::Accepted) {

        DocViewerDialogImpl docView;
        docView.exec(dlg.returnDoc());
    }

}

void mainWindowImpl::reminderAtStartupHack()
{

    showReminder(true);
}
