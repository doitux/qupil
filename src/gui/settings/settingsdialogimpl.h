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
#ifndef SETTINGSDIALOGIMPL_H
#define SETTINGSDIALOGIMPL_H

#include "ui_settings.h"

class ConfigFile;
class myDBHandler;
class mainWindowImpl;

class SettingsDialogImpl: public QDialog, public Ui::SettingsDialog
{
    Q_OBJECT
public:
    SettingsDialogImpl(ConfigFile *c, mainWindowImpl *w);
    ~SettingsDialogImpl();

public slots:

    void loadSettings();
    void saveSettings();
    void getMsgSoundFile();
    void getRemSoundFile();
    void changeTimeTableDayTColor();
    void changeTimeTableDayBColor();
    void changeTimeTableLessonTColor();
    void changeTimeTableLessonBColor();
    void changeTimeTablePupilTColor();
    void changeTimeTablePupilBColor();
    void addLessonLocation();
    void delLessonLocation();
    void addInstrument();
    void delInstrument();
    void addInstrumentSize();
    void delInstrumentSize();
    void addPiecesGenre();
    void delPiecesGenre();
    void piecesGenreItemChanged( QListWidgetItem *current, QListWidgetItem *previous );
    void instrumentsItemChanged( QListWidgetItem *current, QListWidgetItem *previous );
    void instrumentSizesItemChanged( QListWidgetItem *current, QListWidgetItem *previous );
    void lessonLocationsItemChanged( QListWidgetItem *current, QListWidgetItem *previous );


private:
    ConfigFile *myConfig;
    mainWindowImpl *myW;


};

#endif
