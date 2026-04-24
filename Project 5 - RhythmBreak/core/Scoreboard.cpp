#include "Scoreboard.h"

// Calculates and returns the combo-multiplied score
int Scoreboard::comboScore(int score) const
{
    // Set and apply the combo score multiplier
    int mult = 1;
    if      (mCombo >= 100) mult = 8;
    else if (mCombo >= 50)  mult = 4;
    else if (mCombo >= 10)  mult = 2;

    return score * mult;
}

// Update score and combo for a successful hit
void Scoreboard::scoreHit(float timeDiff)
{
    HitStatus currStatus;
    if      (std::fabs(timeDiff) <= PERFECT_WINDOW) currStatus = PERFECT;
    else if (std::fabs(timeDiff) <= GOOD_WINDOW)    currStatus = GOOD;
    else if (std::fabs(timeDiff) <= BAD_WINDOW)     currStatus = BAD;
    else                                            currStatus = MISS;

    scoreStatus(currStatus);
}

// Update score and combo for a miss
void Scoreboard::scoreMiss()
{
    scoreStatus(MISS);
}

// Update scoreboard for a hit or miss
void Scoreboard::scoreStatus(HitStatus status)
{
    // Set last status and increment count
    mPrevStatus = status;
    mStatusCount++;

    switch(status)
    {
    case PERFECT:
        mScore += comboScore(PERFECT_SCORE);
        mCombo++;
        mHitCount++;
        break;

    case GOOD:
        mScore += comboScore(GOOD_SCORE);
        mCombo++;
        mHitCount++;
        break;

    case BAD:
        mScore += BAD_SCORE;  // no combo multiplier
        mCombo  = 0;  // reset combo
        mHitCount++;
        break;

    case MISS:
        mCombo = 0;  // reset combo
        break;
    }

    // Update max combo
    mMaxCombo = std::max(mMaxCombo, mCombo);
}

// Calculate the total hit/miss accuracy
float Scoreboard::getAccuracy() const
{
    if (mStatusCount == 0) return 1.0f;

    // TODO: weighted accuracy for PERFECT/GOOD/BAD
    return static_cast<float>(mHitCount) / static_cast<float>(mStatusCount);
}

// Reset the scoreboard
void Scoreboard::reset()
{
    mScore       = 0;
    mCombo       = 0;
    mMaxCombo    = 0;
    mHitCount    = 0;
    mStatusCount = 0;
    mPrevStatus  = MISS;
}
