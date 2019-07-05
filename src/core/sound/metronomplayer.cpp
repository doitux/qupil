#include "metronomplayer.h"
#include "sdlplayer.h"
#include <SDL/SDL.h>
#include <qtimer.h>
#include <iostream>
#include <qmessagebox.h>

using namespace std;

MetronomPlayer::MetronomPlayer(SDLPlayer *s) : numBeats(4), mySDLPlayer(s)
{
    currentBeat = -1;
    oldBeat = -1;
    bytesLeftToNext = 0;
    for(int i=0; i<32; i++) accent[i]=false;
    last = &clickOne;
}

bool MetronomPlayer::loadSounds(const QString& f1,const QString& f2,const QString& f3)
{
    if( SDL_LoadWAV(f2.toStdString().c_str(), &click.spec, &click.buffer, &click.length) == nullptr ) {
        QMessageBox::critical(nullptr,"Qupil", QString(tr("Could not load %1")).arg(f1));
        return false;
    }
    if( SDL_LoadWAV(f1.toStdString().c_str(), &clickOne.spec, &clickOne.buffer, &clickOne.length)== nullptr ) {
        QMessageBox::critical(nullptr,"Qupil", QString(tr("Could not load %1")).arg(f2));
        return false;
    }
    if( SDL_LoadWAV(f3.toStdString().c_str(), &clickAccent.spec, &clickAccent.buffer,&clickAccent.length) == nullptr ) {
        QMessageBox::critical(nullptr,"Qupil", QString(tr("Could not load %1")).arg(f3));
        return false;
    }
    click.pos = click.length;
    click.bps = click.spec.channels *  click.spec.format == AUDIO_S16 ? 2 : 1;
    clickOne.pos = clickOne.length;
    clickOne.bps = clickOne.spec.channels *  clickOne.spec.format == AUDIO_S16 ? 2 : 1;
    clickAccent.pos = clickAccent.length;
    clickAccent.bps = clickAccent.spec.channels *  clickAccent.spec.format == AUDIO_S16 ? 2 : 1;

    if( click.bps == clickOne.bps && click.bps==clickAccent.bps ) return true;
    QMessageBox::critical(nullptr,"Qupil",tr("Error loading sound files: All sound files must have the same sample format."));
    return false;
}

// Starts or stops the timer
void  MetronomPlayer::changePlayState(bool on)
{
// 	cerr << "Changing player state to ";
    static int fis;
    if(on) {
        if(mySDLPlayer->getAudioEnabled()) {
            mySDLPlayer->audioDone();
            mySDLPlayer->closeAudio();
        }
        initAudio();
        playing=true;
        fis = startTimer(100);
        SDL_PauseAudio(0);
// 		cerr << "on.\n";
    } else {
        killTimer(fis);
        SDL_PauseAudio(1);
        currentBeat = -1;
        oldBeat = -1;
        bytesLeftToNext = 0;
// 		cerr << "off.\n";
        playing = false;
        releaseAudio();
    }
}

// Sets the number of beats
void  MetronomPlayer::setNumBeats(int n)
{
    numBeats = n;
}

#define MIN(x,y) x<y?x:y;

/// Called when the audio device is ready to accept more data
/// \param userdata A pointer to the player class
/// \param stream A pointer to the audio buffer
/// \param len Number of bytes to write
void playerCallback(void *userdata, Uint8 *stream, int len)
{
    MetronomPlayer *p = (MetronomPlayer *)userdata;
    if(!p->isPlaying())
        return;

    int pos = 0;
    // Write silence or sample data until next beat begins, if it does.
    do {
        Uint32 bytesToWrite = MIN(len,p->bytesLeftToNext);
        p->bytesLeftToNext-=bytesToWrite;
        len-=bytesToWrite;
        Uint32 sampleBytesToWrite = MIN(bytesToWrite,p->last->length-p->last->pos);
        memcpy(&stream[pos],&p->last->buffer[p->last->pos],sampleBytesToWrite);
        p->last->pos+=sampleBytesToWrite;
        pos+=sampleBytesToWrite;
        bytesToWrite-=sampleBytesToWrite;
        memset(&stream[pos],0,bytesToWrite);
        pos+=bytesToWrite;
        // Start a new sample?
        if(p->bytesLeftToNext==0) {
            p->currentBeat = (p->currentBeat+1) % p->numBeats;
            if(p->currentBeat==0) p->last = &p->clickOne;
            else if(p->accent[p->currentBeat]) p->last = &p->clickAccent;
            else p->last = &p->click;
            p->last->pos = 0;
            p->bytesLeftToNext = p->beatLengthBytes;

        }
    } while(len>0);
}

void  MetronomPlayer::initAudio()
{
// 	if(SDL_Init(SDL_INIT_AUDIO)==-1)
// 	{
// 		emit AudioInitFail();
// 		return;
// 	}

    SDL_AudioSpec *desired;
    desired=(SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
    desired->freq = click.spec.freq;
    desired->format = click.spec.format;
    desired->channels = click.spec.channels;
    desired->samples = 512;
    desired->callback = playerCallback;
    desired->userdata = this;

    if ( SDL_OpenAudio(desired,nullptr) < 0 ) {
        emit AudioInitFail();
        return;
    }

// 	cerr << "Audio init complete\n";

}

// Release the sound device
void  MetronomPlayer::releaseAudio()
{
    SDL_CloseAudio();
}

void MetronomPlayer::setBPM(int aBpm)
{
    bpm = aBpm;
    // Calculate the number of bytes per beat.
    beatLengthBytes = ((click.spec.freq*60) / bpm) * click.bps;
}

void MetronomPlayer::timerEvent(QTimerEvent*)
{
    if(currentBeat!=oldBeat) {
        oldBeat = currentBeat;
        emit tick(currentBeat);
    }
}

void MetronomPlayer::setAccent(bool yes,int beat)
{
    accent[beat] = yes;
}

