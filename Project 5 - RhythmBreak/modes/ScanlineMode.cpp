#include "ScanlineMode.h"
#include <algorithm>

// Calculates the scanline y-position based on the song time
float ScanlineMode::getScanlineY(float songTime) const
{
    // Calculate the scanline position in the sweep cycle
    float cycleTime = std::fmod(songTime / SWEEP_DURATION, 2.0f);
    if (cycleTime < 0.0f) cycleTime += 2.0f;  // handles negative song times

    float scan = (cycleTime < 1.0f) ? cycleTime : 2.0f - cycleTime;

    return scan * static_cast<float>(SCREEN_HEIGHT);
}

void ScanlineMode::initialise(Section* section, Entity* notes, Scoreboard* scoreboard,
                              int firstNote)
{
    mSection    = section;
    mScoreboard = scoreboard;
    mFirstNote  = firstNote;

    mNoteCount   = static_cast<int>(mSection->circleNotes.size());
    mNoteHead    = 0;
    mStatusTimer = 0.0f;

    // Initialise all note entities and indices
    for (int i = 0; i < mNoteCount; i++) {
        CircleNote& note = mSection->circleNotes[i];
        note.notesIndex = i;

        notes[mFirstNote + i] = Entity(
            note.position,
            { 100.0f, 100.0f },
            "assets/textures/circles/hit-circle.png",
            NOTE
        );
        notes[mFirstNote + i].deactivate();
    }

    // Initialise hit-status textures
    if (!mStatusTexturesLoaded) {
        mStatusTextures[(int)GOOD] = LoadTexture("assets/textures/circles/hit-good.png");
        mStatusTextures[(int)BAD]  = LoadTexture("assets/textures/circles/hit-bad.png");
        mStatusTextures[(int)MISS] = LoadTexture("assets/textures/circles/hit-miss.png");
        mStatusTexturesLoaded      = true;
    }
}

void ScanlineMode::update(Entity* notes, const Input& input, float songTime,
                          float deltaTime)
{
    if (mStatusTimer > 0.0f) mStatusTimer -= deltaTime;

    // Calculate the new scanline position
    mScanlineY = getScanlineY(songTime);

    // Activate and handle note animations
    for (int i = mNoteHead; i < mNoteCount; i++) {
        CircleNote& note   = mSection->circleNotes[i];
        Entity* noteEntity = &notes[mFirstNote + note.notesIndex];

        float timeLeft = note.hitTime - songTime;

        // Activate the note when within the appear window
        if (timeLeft <= 1.5f && !noteEntity->isActive()) noteEntity->activate();

        // Grow the note circle as the scanline approaches
        if (noteEntity->isActive() && timeLeft >= 0.0f) {
            float currDiameter = MAX_RADIUS * 2.0f * (1.0f - (timeLeft / 1.5f));
            noteEntity->setScale({currDiameter, currDiameter});
        }

        if (timeLeft > 0.75f) break;
    }

    // Handle keypress inputs for the active note
    bool keyPressed = input.lanesPressed[0] || input.lanesPressed[1] ||
                      input.lanesPressed[2] || input.lanesPressed[3];

    if (keyPressed && mNoteHead < mNoteCount) {
        CircleNote& note = mSection->circleNotes[mNoteHead];
        float timeDiff   = songTime - note.hitTime;

        // Handle note hits and status scoring
        if (std::fabs(timeDiff) <= Scoreboard::BAD_WINDOW) {
            mScoreboard->scoreHit(timeDiff);

            if      (std::fabs(timeDiff) <= Scoreboard::PERFECT_WINDOW) mPrevHitStatus = PERFECT;
            else if (std::fabs(timeDiff) <= Scoreboard::GOOD_WINDOW)    mPrevHitStatus = GOOD;
            else                                                        mPrevHitStatus = BAD;

            mStatusTimer = 0.2f;
            notes[mFirstNote + note.notesIndex].deactivate();
            mNoteHead++;
        }
    }

    // Handle auto misses
    while (mNoteHead < mNoteCount) {
        CircleNote& note   = mSection->circleNotes[mNoteHead];
        Entity* noteEntity = &notes[mFirstNote + note.notesIndex];

        if (songTime - note.hitTime > GameMode::MISS_WINDOW) {
            if (noteEntity->isActive()) {
                mScoreboard->scoreMiss();
                noteEntity->deactivate();
 
                mPrevHitStatus = MISS;
                mStatusTimer   = 0.2f;
            }
            mNoteHead++;
        } else {
            break;
        }
    }
}

void ScanlineMode::render(Entity* notes)
{
    // Render active note circles
    for (int i = mNoteHead; i < mNoteCount; i++) {
        CircleNote& note   = mSection->circleNotes[i];
        Entity* noteEntity = &notes[mFirstNote + note.notesIndex];

        if (noteEntity->isActive()) noteEntity->render();
    }
    
    // Render the scanline
    DrawLineEx(
        { 0.0f, mScanlineY },
        { static_cast<float>(SCREEN_WIDTH), mScanlineY },
        3.0f, { 255, 255, 255, 200 }
    );

    // Faint glow effect!
    DrawLineEx(
        { 0.0f, mScanlineY - 4.0f },
        { static_cast<float>(SCREEN_WIDTH), mScanlineY - 4.0f },
        1.0f, {255, 255, 255, 60}
    );
    DrawLineEx(
        { 0.0f, mScanlineY + 4.0f },
        { static_cast<float>(SCREEN_WIDTH), mScanlineY + 4.0f },
        1.0f, {255, 255, 255, 60}
    );
}

void ScanlineMode::drawUI()
{
    if (mStatusTimer > 0.0f && mStatusTexturesLoaded &&
        mPrevHitStatus != PERFECT) {
        Texture2D& texture = mStatusTextures[(int)mPrevHitStatus];
    
        float tWidth  = static_cast<float>(texture.width) * 1.1f;
        float tHeight = static_cast<float>(texture.height) * 1.1f;
    
        Rectangle src = {
            0.0f, 0.0f,
            static_cast<float>(texture.width),
            static_cast<float>(texture.height)
        };
    
        Rectangle dst = {
            mLastHitPos.x - tWidth / 2.0f,
            mLastHitPos.y - tHeight / 2.0f,
            tWidth, tHeight
        };

        DrawTexturePro(texture, src, dst, {0.0f, 0.0f}, 0.0f, Fade(WHITE, mStatusTimer / 0.2f));
    }
}

void ScanlineMode::shutdown()
{
    if (mStatusTexturesLoaded) {
        for (int i = 0; i < 4; i++) {
            UnloadTexture(mStatusTextures[i]);
        }
        mStatusTexturesLoaded = false;
    }

    mSection    = nullptr;
    mScoreboard = nullptr;
    mNoteCount  = 0;
    mNoteHead   = 0;
}