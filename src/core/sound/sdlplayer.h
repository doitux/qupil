//
// C++ Interface: sdlplayer
//
// Description:
//
//
// Author: FThauer FHammer <webmaster@pokerth.net>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#ifndef SDLPLAYER_H
#define SDLPLAYER_H

#include "configfile.h"

#if (defined __APPLE__)
#include <SDL_mixer.h>
#else
#include <SDL/SDL_mixer.h>
#endif

#include <iostream>
#include <string>
#include "metronomplayer.h"
#include "metronomdialogimpl.h"

// struct Mix_Chunk;

/**
 @author FThauer FHammer <webmaster@pokerth.net>
*/
class SDLPlayer : public QObject
{
    Q_OBJECT

public:
    SDLPlayer(ConfigFile*);

    ~SDLPlayer();

    bool initAudio();
    void playSound(QString, int, bool = false);
    void audioDone();
    void closeAudio();
    void loadMetronomTicks(QString audioString1, QString audioString2, QString audioString3);
    void playMetronomTick(int);

    bool getAudioEnabled() const {
        return audioEnabled;
    };

    void setMetronomPlayer ( MetronomPlayer* theValue ) {
        myMetronomPlayer = theValue;
    }
    void setMyMetronomDialog ( MetronomDialogImpl* theValue ) {
        myMetronomDialog = theValue;
    }

private:

    Mix_Chunk *sound;
    Mix_Chunk *tick1;
    Mix_Chunk *tick2;
    Mix_Chunk *tick3;

    unsigned char *soundData;
    int currentChannel;

    bool audioEnabled;

    ConfigFile *myConfig;
    QString myAppDataPath;
    MetronomPlayer *myMetronomPlayer;
    MetronomDialogImpl *myMetronomDialog;

};

#endif
