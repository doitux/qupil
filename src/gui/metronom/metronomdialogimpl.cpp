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
#include "metronomdialogimpl.h"

#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "configfile.h"
#include "mainwindowimpl.h"
#include "sdlplayer.h"
#include "metronomplayer.h"

MetronomDialogImpl::MetronomDialogImpl(ConfigFile *c, mainWindowImpl *w, SDLPlayer *p, MetronomPlayer *m)
    : myConfig(c), myW(w), mySDLPlayer(p), myMetronomPlayer(m), startCheckTimer(false), checkTimerCounter(0)
{
    setupUi(this);

    initsuccess = true;


    myMetronomPlayer->setBPM(spinBox_bpm->value());

// 	mySDLPlayer->loadMetronomTicks("s01","s02","s03");

    checkLastTime = new QTime();
    timer = new QTimer();
// 	metronomTimer = new QTimer();

    connect( pushButton_play, SIGNAL( toggled(bool) ), myMetronomPlayer, SLOT( changePlayState(bool) ) );
    connect( pushButton_playTune, SIGNAL( clicked(bool) ), this, SLOT( playTune() ) );
    connect( pushButton_checkTempo, SIGNAL( clicked(bool) ), this, SLOT( checkTempo() ) );
    connect( spinBox_bpm, SIGNAL (valueChanged(int) ), this, SLOT (changeTempo(int) ) );
    connect( spinBox_beat, SIGNAL (valueChanged(int) ), this, SLOT (changeBeats(int) ) );
    connect( checkBox_beat, SIGNAL (toggled(bool) ), this, SLOT(beatOnOff(bool) ) );
    connect( myMetronomPlayer,SIGNAL (AudioInitFail()),this, SLOT(audioInitFail()));
    connect( timer, SIGNAL (timeout()), this, SLOT ( checkResetTempoChecker() ) );
// 	connect( metronomTimer, SIGNAL (timeout()), this, SLOT ( playMetronomTick() ) );

}

MetronomDialogImpl::~MetronomDialogImpl() {}

void MetronomDialogImpl::show()
{
    retranslateUi(this);
    timer->start(1000);
    QDialog::show();
}

void MetronomDialogImpl::changeTempo(int t)
{
// 	metronomTimer->setInterval(60000/spinBox_bpm->value());
    myMetronomPlayer->setBPM(t);
}

void MetronomDialogImpl::audioInitFail()
{
    QMessageBox::critical(this,"Qupil","Konnte die Soundkarte nicht initialisieren.");
}

void MetronomDialogImpl::checkTempo()
{
    if(!startCheckTimer) {
        checkLastTime->setHMS(QTime::currentTime().hour(), QTime::currentTime().minute(), QTime::currentTime().second(), QTime::currentTime().msec());
        //start the real timer
        realTimer.reset();
        realTimer.start();

        startCheckTimer = true;
    } else {
        spinBox_bpm->setValue(60000/realTimer.elapsed().total_milliseconds());
        realTimer.reset();
        realTimer.start();
    }
}

void MetronomDialogImpl::checkResetTempoChecker()
{
    if(checkLastTime->msecsTo(QTime::currentTime()) > 1700) startCheckTimer = false;
}

void MetronomDialogImpl::changeBeats(int b)
{
    myMetronomPlayer->setNumBeats(b);
}

void MetronomDialogImpl::beatOnOff(bool b)
{
    if(b) {
        if(!myMetronomPlayer->loadSounds(QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str())+"sounds/s1.wav", QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str())+"sounds/s2.wav",QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str())+"sounds/s3.wav")) {
            initsuccess = false;
            return;
        }
        myMetronomPlayer->setNumBeats(spinBox_beat->value());

    } else {
        if(!myMetronomPlayer->loadSounds(QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str())+"sounds/s2.wav", QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str())+"sounds/s2.wav",QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str())+"sounds/s3.wav")) {
            initsuccess = false;
            return;
        }
    }
}

void MetronomDialogImpl::playTune()
{
    if(comboBox_tuneTone->currentIndex() == 0) {	//A
        switch(spinBox_tunePitch->value()) {
        case 438:
            mySDLPlayer->playSound(QString("a438.ogg"),10);
            break;
        case 439:
            mySDLPlayer->playSound(QString("a439.ogg"),10);
            break;
        case 440:
            mySDLPlayer->playSound(QString("a440.ogg"),10);
            break;
        case 441:
            mySDLPlayer->playSound(QString("a441.ogg"),10);
            break;
        case 442:
            mySDLPlayer->playSound(QString("a442.ogg"),10);
            break;
        case 443:
            mySDLPlayer->playSound(QString("a443.ogg"),10);
            break;
        case 444:
            mySDLPlayer->playSound(QString("a444.ogg"),10);
            break;
        case 445:
            mySDLPlayer->playSound(QString("a445.ogg"),10);
            break;
        }
    } else { //B
        switch(spinBox_tunePitch->value()) {
        case 438:
            mySDLPlayer->playSound(QString("b438.ogg"),10);
            break;
        case 439:
            mySDLPlayer->playSound(QString("b439.ogg"),10);
            break;
        case 440:
            mySDLPlayer->playSound(QString("b440.ogg"),10);
            break;
        case 441:
            mySDLPlayer->playSound(QString("b441.ogg"),10);
            break;
        case 442:
            mySDLPlayer->playSound(QString("b442.ogg"),10);
            break;
        case 443:
            mySDLPlayer->playSound(QString("b443.ogg"),10);
            break;
        case 444:
            mySDLPlayer->playSound(QString("b444.ogg"),10);
            break;
        case 445:
            mySDLPlayer->playSound(QString("b445.ogg"),10);
            break;
        }
    }
}

void MetronomDialogImpl::changePlayState(bool b)
{
    if(b) {
        metronomTimer->start(60000/spinBox_bpm->value());
        playMetronomTick();
    } else metronomTimer->stop();
}

void MetronomDialogImpl::playMetronomTick()
{
    mySDLPlayer->playMetronomTick(0);
}

void MetronomDialogImpl::reject()
{
    pushButton_play->setChecked(false);
    mySDLPlayer->closeAudio();
    QDialog::reject();
}


