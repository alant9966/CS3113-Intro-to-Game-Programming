#include "../cs3113/cs3113.h"

#ifndef CLOCK_H
#define CLOCK_H

class Clock
{
private:
    Music mBgm     = {};
    float mOffset  = 0.0f;
    bool  mPlaying = false;

public:
    void initialise(Music bgm, float offset = 0.0f);
    void update();
    void play();
    void pause();

    float getSongTime() const;
    bool  isFinished()  const;

    Music& getBgm() { return mBgm; }

    void setOffset(float offset) { mOffset = offset; }
};

#endif