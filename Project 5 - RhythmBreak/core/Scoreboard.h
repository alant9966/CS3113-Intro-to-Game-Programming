#include "Beatmap.h"

#ifndef SCOREBOARD_H
#define SCOREBOARD_H

enum HitStatus { PERFECT, GOOD, BAD, MISS };

class Scoreboard
{
private:
    int mScore       = 0;
    int mCombo       = 0;
    int mMaxCombo    = 0;
    int mTotalNotes  = 0;
    int mHitCount    = 0;
    int mStatusCount = 0;

    HitStatus mPrevStatus = MISS;

    // Score with combo multiplier
    int comboScore(int score) const;

public:
    // Timing windows for hit statuses (sec)
    static constexpr float PERFECT_WINDOW = 0.040f;
    static constexpr float GOOD_WINDOW    = 0.090f;
    static constexpr float BAD_WINDOW     = 0.150f;

    // Score values for each hit status
    static constexpr int PERFECT_SCORE = 300;
    static constexpr int GOOD_SCORE    = 100;
    static constexpr int BAD_SCORE     = 50;
    static constexpr int MISS_SCORE    = 0;

    void scoreHit(float timeDiff);
    void scoreMiss();
    void scoreStatus(HitStatus status);

    void  reset();  // for song restart
    float getAccuracy() const;
    
    int       getScore()      const { return mScore;                   }
    int       getCombo()      const { return mCombo;                   }
    int       getMaxCombo()   const { return mMaxCombo;                }
    int       getTotalNotes() const { return mTotalNotes;              }
    int       getHitCount()   const { return mHitCount;                }
    HitStatus getPrevStatus() const { return mPrevStatus;              }
    bool      isFullCombo()   const { return mMaxCombo == mTotalNotes; }

    void setMaxCombo(int combo)   { mMaxCombo   = combo; }
    void setTotalNotes(int count) { mTotalNotes = count; }
};

#endif