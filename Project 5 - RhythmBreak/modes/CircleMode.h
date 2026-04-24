#include "../core/Scoreboard.h"
#include "GameMode.h"

#ifndef CIRCLEMODE_H
#define CIRCLEMODE_H

class CircleMode : public GameMode
{
private:
    Section*    mSection    = nullptr;
    Scoreboard* mScoreboard = nullptr;
    int         mFirstNote  = 0;

    Texture2D mStatusTextures[4];  // textures for hit/miss status text
    bool      mStatusTexturesLoaded = false;

    int mNoteCount      = 0;
    int mNoteHead       = 0;  // active note index
    int mApproachOffset = 0;

    // Display note hit/miss status
    HitStatus mPrevHitStatus = MISS;
    float     mStatusTimer   = 0.0f;
    Vector2   mLastHitPos    = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

    // Checks if the mouse is hovering over a note
    bool isMouseOverNote(const Entity* note, Vector2 mousePos) const;

public:
    void initialise(Section* section, Entity* notes, Scoreboard* scoreboard,
                    int firstNote, int approachOffset);

    void update(Entity* notes, const Input& input, float songTime,
                float deltaTime) override;
    void render(Entity* notes) override;
    void drawUI() override;
    void shutdown() override;

    int getNoteCount() const override
    {
        return mSection ? static_cast<int>(mSection->circleNotes.size()) : 0;
    }
};

#endif