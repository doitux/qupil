//
// C++ Implementation: sdlplayer
//
// Description:
//
//
// Author: FThauer FHammer <webmaster@pokerth.net>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sdlplayer.h"

#if (defined __APPLE__)
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif

using namespace std;

SDLPlayer::SDLPlayer(ConfigFile *c)
    : soundData(NULL), currentChannel(0) , audioEnabled(0), myConfig(c)
{
    SDL_Init(SDL_INIT_AUDIO);


    myAppDataPath = QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str());
}


SDLPlayer::~SDLPlayer()
{
    closeAudio();
    SDL_Quit();
}

bool SDLPlayer::initAudio()
{

    if (!audioEnabled) {
        int		audio_rate = 44100;
        Uint16	audio_format = AUDIO_S16; /* 16-bit stereo */
        int		audio_channels = 2;
        int		audio_buffers = 4096;
        sound = NULL;

        if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) == 0) {
            Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
            audioEnabled = 1;
            return true;
        } else {
            qDebug() << "Fehler beim Laden des Audiogerätes";
            return false;
        }
    }
    return true;
}

void SDLPlayer::playSound(QString audioString, int volume, bool fullpath)
{

    if(myMetronomPlayer->isPlaying()) myMetronomDialog->pushButton_play->setChecked(false);

    if(!initAudio()) {
        QMessageBox::critical(myMetronomDialog,"Qupil","Konnte die Soundkarte nicht initialisieren.");
    } else {
        QString myFile;
        if(fullpath) {
            myFile = audioString;
        } else {
            myFile= myAppDataPath+"sounds/"+audioString;
        }
        audioDone();
        sound = Mix_LoadWAV(myFile.toStdString().c_str());
        if(!sound) {
            qDebug() << "Mix_LoadWAV: " << Mix_GetError();
            // handle error
        }
        // set channel 0 to settings volume
        Mix_Volume(0,volume*10);

        currentChannel = Mix_PlayChannel(-1, sound,0);
    }
}

void SDLPlayer::loadMetronomTicks(QString audioString1, QString audioString2, QString audioString3)
{

    tick1=Mix_LoadWAV(QString(myAppDataPath+"sounds/"+audioString1+".wav").toStdString().c_str());
    if(!tick1) {
        printf("Mix_LoadWAV: %s\n", Mix_GetError());
        // handle error
    }
    tick2=Mix_LoadWAV(QString(myAppDataPath+"sounds/"+audioString2+".wav").toStdString().c_str());
    if(!tick2) {
        printf("Mix_LoadWAV: %s\n", Mix_GetError());
        // handle error
    }
    tick3=Mix_LoadWAV(QString(myAppDataPath+"sounds/"+audioString3+".wav").toStdString().c_str());
    if(!tick3) {
        printf("Mix_LoadWAV: %s\n", Mix_GetError());
        // handle error
    }

}

void SDLPlayer::audioDone()
{

    if(audioEnabled) {
        Mix_HaltChannel(currentChannel);
        Mix_FreeChunk(sound);
        sound = NULL;
        delete[] soundData;
        soundData = NULL;
    }
}

void SDLPlayer::closeAudio()
{

    if(audioEnabled) {
        audioDone();
        Mix_CloseAudio();
        audioEnabled = false;
    }
}

void SDLPlayer::playMetronomTick(int /*tick*/)
{
    if(Mix_PlayChannel(-1, tick2, 0)==-1) {
        printf("Mix_PlayChannel: %s\n",Mix_GetError());
        // may be critical error, or maybe just no channels were free.
        // you could allocated another channel in that case...
    }

}
