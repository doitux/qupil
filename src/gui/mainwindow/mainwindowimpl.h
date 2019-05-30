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
#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include "ui_mainwindow.h"

class ConfigFile;
class myDBHandler;
class SDLPlayer;
class MetronomPlayer;
class QProgressDialog;
class QSqlQueryModel;

class BirthdaysDialogImpl;
class SettingsDialogImpl;
class ConcertManagerDialogImpl;
class InstrumentManagerDialogImpl;
class MetronomDialogImpl;
class SheetMusicLibraryDialogImpl;
class AboutQupilDialogImpl;
class CsvImportFieldsDialogImpl;
class BuildTimeTableDoc;
class ReminderDialog;

class mainWindowImpl: public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
public:
    mainWindowImpl(ConfigFile *c, myDBHandler *d);
    ~mainWindowImpl();

    myDBHandler* getMyDb() const {
        return myDb;
    }

    SDLPlayer* getMySDLPlayer() const {
        return mySDLPlayer;
    }


    void checkForOldDataStructure();
    void updateConvertOldDataProgress(int value, QString text);

public slots:

    void callSettingsDialog();
    void callConcertManagerDialog();
    void callInstrumentManagerDialog();
    void callMetronomDialog();
    void callSheetMusicLibraryDialog();
    void callAboutQupilDialog();
    void birthdayReminder();
    void callBirthdaysDialog();
    void leftListModeChanged(int);
    void addNewLesson();
    void addNewPupil();
    void addNewPupilsFromCsv();
    void lessonEndMsgPlayer();
    void rightTabsChanged(int);
    void createBackup();
    void restoreBackup();
    void showTimeTableDoc();
    void checkForRecitalPupils();
    void checkForEnsemblePupils();
    void refreshTimeTableStats(int, int);
    void callReminderDialog();
    void checkForProgramStartReminder();
    void checkIfReminderTimerNeeded();
    void showReminder(bool directlyAfterStartup = false);
    void reminderAtStartupHack();
    bool eventFilter(QObject *obj, QEvent *event);
    void saveWindowGeometry();
    void restoreWindowGeometry();
    void callBuildDayViewDialog();

private:
    QTranslator qtTranslator;
    QTranslator myAppTranslator;

    ConfigFile *myConfig;
    myDBHandler *myDb;
    SDLPlayer *mySDLPlayer;
    MetronomPlayer *myMetronomPlayer;

    QTimer *lessonEndMsgTimer;
    QTimer *reminderTimer;
    QLabel *statusBarStats;
    QLabel *statusBarSpace;
    QLabel *statusBarLehrsaiten;

    QProgressDialog *convertOldDataProgress;
    QSqlQueryModel *timeTableModel;

    QString thisReminder1TimeAlreadyDone;
    QString thisReminder2TimeAlreadyDone;

    int reminderAtLessonStartCounter;

    SettingsDialogImpl *mySettingsDialog;
    ConcertManagerDialogImpl *myConcertManagerDialog;
    InstrumentManagerDialogImpl *myInstrumentManagerDialog;
    BirthdaysDialogImpl *myBirthdaysDialog;
    MetronomDialogImpl *myMetronomDialog;
    SheetMusicLibraryDialogImpl *mySheetMusicLibraryDialog;
    AboutQupilDialogImpl *myAboutQupilDialog;
    CsvImportFieldsDialogImpl *myCsvImportFieldsDialog;
    BuildTimeTableDoc *myBuildTimeTableDoc;

};

#endif
