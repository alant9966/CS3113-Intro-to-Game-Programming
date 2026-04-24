#include "../core/Scoreboard.h"
#include "GameMode.h"

#ifndef SCANLINEMODE_H
#define SCANLINEMODE_H

class ScanlineMode : public GameMode
{
private:
    Section*    mSection    = nullptr;
    Scoreboard* mScoreboard = nullptr;
    int         mFirstNote  = 0;

    int mNoteCount = 0;
    int mNoteHead  = 0;

    Texture2D mStatusTextures[4];  // textures for hit/miss status text
    bool      mStatusTexturesLoaded = false;

    // Scanline position and direction
    float mScanlineY   = 0.0f;
    int   mScanlineDir = 1; // 1 = down, -1 = up

    // Display note hit/miss status
    HitStatus mPrevHitStatus = MISS;
    float     mStatusTimer   = 0.0f;
    Vector2   mLastHitPos    = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

public:
    static constexpr float SWEEP_DURATION = 2.0f;  // seconds
    static constexpr float MAX_RADIUS = 100.0f;

    void initialise(Section* section, Entity* notes, Scoreboard* scoreboard,
                    int firstNote);

    void update(Entity* notes, const Input& input, float songTime,
                float deltaTime) override;
    void render(Entity* notes) override;
    void drawUI() override;
    void shutdown() override;

    int getNoteCount() const override
    {
        return mSection ? static_cast<int>(mSection->circleNotes.size()) : 0;
    }

private:
    // Calculate the scanline y-position
    float getScanlineY(float songTime) const;
};

#endif