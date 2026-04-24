#include "FallingMode.h"
#include <algorithm>

// Initializes lane note lists and sorts by hit time
void FallingMode::initialise(Section* section, Entity* notes, Scoreboard* scoreboard,
                             int firstNote)
{
    mSection    = section;
    mFirstNote  = firstNote;
    mScoreboard = scoreboard;

    // Reset note lists for each lane (per section)
    std::fill(mLaneHeads, mLaneHeads + 4, 0);
    for (auto& laneNotes : mLaneNoteIndices) {
        laneNotes.clear();
    }

    // For each note, assign its order and set its initial position
    for (int i = 0; i < static_cast<int>(mSection->fallingNotes.size()); i++) {
        // Assign the note's index and order
        FallingNote& note = mSection->fallingNotes[i];
        note.notesIndex = i;
        
        notes[firstNote + i] = Entity(
            { -200.0f, -200.0f },
            { LANE_WIDTH, NOTE_HEIGHT },
            (note.lane % 2 == 0) ? "assets/textures/falling/falling-note-white.png"
                                 : "assets/textures/falling/falling-note-blue.png",
            NOTE
        );

        // Assign initial position and state
        Entity* currNote = &notes[mFirstNote + i];
        currNote->setPosition({laneCenters[note.lane], -NOTE_HEIGHT});
        currNote->deactivate();  // activated when needed

        mLaneNoteIndices[note.lane].push_back(i);
    }

    // Sort lanes by note time
    for (int l = 0; l < 4; l++) {
        auto& indices = mLaneNoteIndices[l];
        std::sort(indices.begin(), indices.end(), [this](int a, int b) {
            return mSection->fallingNotes[a].hitTime < mSection->fallingNotes[b].hitTime;
        });
    }

    // Initialise lanes and hit lines
    for (int lane = 0; lane < 4; lane++) {
        mLanes[lane] = Entity(
            { laneCenters[lane], 405.0f },
            { LANE_WIDTH, (float)SCREEN_HEIGHT },
            "assets/textures/falling/lane.png",
            LANE
        );
        mLanes[lane].setTint({ 255, 255, 255, 128 });

        mHitLines[lane] = Entity(
            { laneCenters[lane], HIT_LINE_Y },
            { LANE_WIDTH, NOTE_HEIGHT },
            "assets/textures/falling/stage-hint.png",
            EFFECT
        );
    }

    // Initialise status textures
    if (!mStatusTexturesLoaded) {
        mStatusTextures[(int)PERFECT] = LoadTexture("assets/textures/falling/hit-perfect.png");
        mStatusTextures[(int)GOOD]    = LoadTexture("assets/textures/falling/hit-good.png");
        mStatusTextures[(int)BAD]     = LoadTexture("assets/textures/falling/hit-bad.png");
        mStatusTextures[(int)MISS]    = LoadTexture("assets/textures/falling/hit-miss.png");
        mStatusTexturesLoaded = true;
    }

    mStatusTimer = 0.0f;
}

// Update note positions and handle misses
void FallingMode::update(Entity* notes, const Input& input, float songTime, 
                         float deltaTime)
{
    if (mStatusTimer > 0.0f) mStatusTimer -= deltaTime;

    // Update lanes (for potential animation or effects)
    for (int i = 0; i < 4; i++) {
        mLanes[i].update(deltaTime, nullptr, 0);
    }

    // Handle key press inputs
    for (int i = 0; i < 4; i++) {
        if (input.lanesPressed[i]) processInput(i, songTime, notes);
    }

    // Update note positions and activity
    for (int l = 0; l < 4; l++){
        const auto& indices = mLaneNoteIndices[l];

        for (int li = mLaneHeads[l]; li < static_cast<int>(indices.size()); li++)
        {
            FallingNote& note  = mSection->fallingNotes[indices[li]];
            Entity* noteEntity = &notes[mFirstNote + note.notesIndex];

            // Calculate the y-position of the note as it falls
            float noteY = HIT_LINE_Y - (note.hitTime - songTime) * SCROLL_SPEED;

            // Activate the entity when it enters into the window
            if (!noteEntity->isActive() && noteY > -850.0f) noteEntity->activate();

            // Set note position and update entity
            if (noteEntity->isActive()){
                noteEntity->setPosition({laneCenters[note.lane], noteY});
                noteEntity->update(deltaTime, nullptr, 0);
            }

            // Auto-miss when note passes miss window
            if (songTime > note.hitTime + GameMode::MISS_WINDOW) {
                // Register a miss and deactivate the note
                if (noteEntity->isActive()) {
                    mScoreboard->scoreMiss();
                    noteEntity->deactivate();

                    mPrevHitStatus = MISS;
                    mStatusTimer = 0.2f;
                }
                mLaneHeads[l]++;
            }

            if (noteY > 810 + NOTE_HEIGHT * 2.0f) break;
        }
    }
}

// Check and handle keypresses in each lane
void FallingMode::processInput(int lane, float songTime, Entity* notes)
{
    const auto& indices = mLaneNoteIndices[lane];

    // No more notes remaining - no need to process input
    if (mLaneHeads[lane] >= static_cast<int>(indices.size())) return;

    FallingNote& note = mSection->fallingNotes[indices[mLaneHeads[lane]]];
    float timeDiff = songTime - note.hitTime;

    // Ignore input if hit window has passed
    if (std::fabs(timeDiff) > Scoreboard::BAD_WINDOW) return;

    // Register the hit and determine its status
    mScoreboard->scoreHit(timeDiff);
    mPrevHitStatus = (std::fabs(timeDiff) <= Scoreboard::PERFECT_WINDOW) ? PERFECT
                   : (std::fabs(timeDiff) <= Scoreboard::GOOD_WINDOW)    ? GOOD
                                                                         : BAD;
    mStatusTimer = 0.2f;

    // Deactivate the note entity and update the active note
    notes[mFirstNote + note.notesIndex].deactivate();
    mLaneHeads[lane]++;
}

// Render lanes, effects, and active notes
void FallingMode::render(Entity* notes)
{   
    // Render lanes
    for (int i = 0; i < 4; i++) {
        mLanes[i].render();
    }

    // Render hit lines
    for (int lane = 0; lane < 4; lane++) {
        mHitLines[lane].render();
    }
 
    // Render active notes
    if (!mSection) return;
    for (int i = 0; i < static_cast<int>(mSection->fallingNotes.size()); i++) {
        notes[mFirstNote + i].render();
    }
}

// Draw hit/miss status text and UI elements
void FallingMode::drawUI()
{
    if (mStatusTimer > 0.0f && mStatusTexturesLoaded) {
        Texture2D& texture = mStatusTextures[(int)mPrevHitStatus];

        float tWidth  = static_cast<float>(texture.width) * 1.1f;
        float tHeight = static_cast<float>(texture.height) * 1.1f;

        Rectangle src = {
            0.0f, 0.0f,
            static_cast<float>(texture.width),
            static_cast<float>(texture.height)
        };

        Rectangle dst = {
            SCREEN_WIDTH / 2.0f - tWidth / 2.0f,
            SCREEN_HEIGHT / 2.0f - tHeight / 2.0f,
            tWidth, tHeight
        };

        DrawTexturePro(texture, src, dst, {0.0f, 0.0f}, 0.0f, Fade(WHITE, mStatusTimer / 0.2f));
    }
}

void FallingMode::shutdown()
{
    if (mStatusTexturesLoaded) {
        for (int i = 0; i < 4; i++) {
            UnloadTexture(mStatusTextures[i]);
        }
        mStatusTexturesLoaded = false;
    }

    for (int i = 0; i < 4; i++) {
        mLanes[i]    = Entity();
        mHitLines[i] = Entity();
    }
}
