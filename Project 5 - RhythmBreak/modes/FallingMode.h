#include "../core/Scoreboard.h"
#include "GameMode.h"

#ifndef FALLINGMODE_H
#define FALLINGMODE_H

class FallingMode : public GameMode
{
private:
    Section*    mSection    = nullptr;
    Scoreboard* mScoreboard = nullptr;
    int         mFirstNote  = 0;  // index of the first note

    Entity mLanes[4];              // lane entities (D/F/J/K columns)
    Entity mHitLines[4];           // hit lines for each lane
    Texture2D mStatusTextures[4];  // textures for hit/miss status text
    bool mStatusTexturesLoaded = false;

    // Head index (active note) for each lane
    int mLaneHeads[4] = { 0, 0, 0, 0 };

    // Note lists for each lane
    std::vector<int> mLaneNoteIndices[4];

    // Display note hit/miss status
    HitStatus mPrevHitStatus = MISS;
    float     mStatusTimer   = 0.0f;

    // Checks for keypresses in each lane
    void processInput(int lane, float songTime, Entity* notes);

public:  
    static constexpr float SCROLL_SPEED = 565.0f;  // px/sec
    static constexpr float LANE_WIDTH   = 120.0f;  // = note width
    static constexpr float NOTE_HEIGHT  = 35.0f;
    static constexpr float HIT_LINE_Y   = 720.0f;

    static constexpr float laneCenters[4] = { 540.0f, 660.0f, 780.0f, 900.0f };  // centered, no gaps

    // Initializes lane note lists and sorts by hit time
    void initialise(Section* section, Entity* notes, Scoreboard* scoreboard,
        int firstNote);

    // Update note positions and handle misses
    void update(Entity* notes, const Input& input, float songTime, 
        float deltaTime);

    // Draw lanes, effects, and active notes
    void render(Entity* notes);

    // Draw hit/miss status text and UI elements
    void drawUI();

    void shutdown();

    // Return the number of notes in the section
    int getNoteCount() const
    {
        return mSection ? static_cast<int>(mSection->fallingNotes.size()) : 0;
    }
};

#endif