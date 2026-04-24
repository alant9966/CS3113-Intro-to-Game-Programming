#include "Clock.h"

// Initialise music and calibration offset (seconds)
void Clock::initialise(Music bgm, float offset)
{
    mBgm     = bgm;
    mOffset  = offset;
    mPlaying = false;
}

// Update the music stream
void Clock::update()
{
    if (mPlaying) UpdateMusicStream(mBgm);
}

// Start the music stream
void Clock::play()
{
    if (!mPlaying) {
        PlayMusicStream(mBgm);
        mPlaying = true;
    } else {
        ResumeMusicStream(mBgm);
    }
}

// Pause the music stream
void Clock::pause()
{
    PauseMusicStream(mBgm);
    mPlaying = false;
}

// Return the current song progress (for beatmap timing)
float Clock::getSongTime() const
{
    return GetMusicTimePlayed(mBgm) + mOffset;
}

// Check if the song has finished
bool Clock::isFinished() const
{
    return GetMusicTimePlayed(mBgm) >= GetMusicTimeLength(mBgm) - 0.05f;
}
