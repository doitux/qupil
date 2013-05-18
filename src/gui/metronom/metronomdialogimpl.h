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
#ifndef METRONOMDIALOGIMPL_H
#define METRONOMDIALOGIMPL_H

#include "ui_metronom.h"
#include <core/third_party/boost/timers.hpp>

class ConfigFile;
class myDBHandler;
class mainWindowImpl;
class MetronomPlayer;
class QTime;
class SDLPlayer;

class MetronomDialogImpl: public QDialog, public Ui::MetronomDialog
{
    Q_OBJECT
public:
    MetronomDialogImpl(ConfigFile *c, mainWindowImpl *w, SDLPlayer *p, MetronomPlayer *m);
    ~MetronomDialogImpl();

public slots:

    void show();
    void reject();
    void changeTempo(int);
    void audioInitFail();
    void checkTempo();
    void changeBeats(int);
    void changePlayState(bool);
    void beatOnOff(bool);
    void checkResetTempoChecker();
    void playTune();
    void playMetronomTick();

private:
    ConfigFile *myConfig;
    mainWindowImpl *myW;
    SDLPlayer *mySDLPlayer;
    MetronomPlayer *myMetronomPlayer;
    bool initsuccess;
    QTime *checkLastTime;
    QTimer *timer;
    QTimer *metronomTimer;
    bool startCheckTimer;
    int checkTimerCounter;
    int checkTimerSum;
    boost::timers::portable::microsec_timer realTimer;

};

#endif
