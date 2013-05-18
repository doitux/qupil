#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <SDL/SDL.h>
#include <qwidget.h>

struct Sample {
    SDL_AudioSpec spec;
    Uint32 length;			// Length in bytes
    Uint8 *buffer;
    int pos;				// Position in bytes
    Uint8 bps;				// Bytes per sample
};

class QTimer;
class SDLPlayer;

class MetronomPlayer : public QObject
{
    Q_OBJECT

public:
    MetronomPlayer(SDLPlayer*);
    //bool loadClick(char *file);
    //bool loadClickOne(char *file);
    //bool loadClickAccent(char *file);

    bool loadSounds(const QString& f1,const QString& f2,const QString& f3);


    int getBPM() {
        return bpm;
    }
    virtual void timerEvent( QTimerEvent *);

    int pendingBeat;
    int currentPos;
    Sample click,clickOne,clickAccent;
    Sample *last;
    int beatLengthBytes;				// Beat length in bytes
    int bytesLeftToNext;				// Bytes left to next beat
    int currentBeat;
    int numBeats;
    bool accent[32];

signals:
    void tick(int);
    void AudioInitFail();

public slots:
    // Starts or stops the timer
    void changePlayState(bool);
    // Sets the number of beats
    void setNumBeats(int);
    // Returns the number of beats
    int getNumBeats() {
        return numBeats;
    }
    // Toggle the accented state of the given beat
    void setAccent(bool yes,int beat);
    // Set number of beats per minute
    void setBPM(int);
    // Grab the sound device and initialize it.
    void initAudio();
    // Release the sound device
    void releaseAudio();

    bool isPlaying() {
        return playing;
    }

private:
    bool playing;

    QTimer *timer;
    int bpm;
    int oldBeat;

    SDLPlayer *mySDLPlayer;
};

#endif
