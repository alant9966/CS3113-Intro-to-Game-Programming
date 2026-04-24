#include "CircleMode.h"

void CircleMode::initialise(Section* section, Entity* notes, Scoreboard* scoreboard, 
                            int firstNote, int approachOffset)
{
    mSection        = section;
    mScoreboard     = scoreboard;
    mFirstNote      = firstNote;
    mApproachOffset = approachOffset;

    mNoteHead    = 0;
    mStatusTimer = 0.0f;
    mNoteCount   = static_cast<int>(mSection->circleNotes.size());

    // Initialize hit and approach circle entities for each note
    for (int i = 0; i < mNoteCount; i++) {
        CircleNote& note = mSection->circleNotes[i];
        note.notesIndex = i;

        // Hit circles
        notes[mFirstNote + i] = Entity(
            note.position,
            { 150.0f, 150.0f },
            "assets/textures/circles/hit-circle.png",
            NOTE
        );
        notes[mFirstNote + i].deactivate();

        // Approach circles
        notes[mApproachOffset + i] = Entity(
            note.position,
            { 600.0f, 600.0f },
            "assets/textures/circles/approach-circle.png",
            EFFECT
        );
        notes[mApproachOffset + i].deactivate();
    }

    // Initialise status textures
    if (!mStatusTexturesLoaded) {
        mStatusTextures[(int)GOOD] = LoadTexture("assets/textures/circles/hit-good.png");
        mStatusTextures[(int)BAD]  = LoadTexture("assets/textures/circles/hit-bad.png");
        mStatusTextures[(int)MISS] = LoadTexture("assets/textures/circles/hit-miss.png");
        mStatusTexturesLoaded = true;
    }
}

void CircleMode::update(Entity* notes, const Input& input, float songTime, 
                        float deltaTime)
{
    if (mStatusTimer > 0.0f) mStatusTimer -= deltaTime;

    // Handle updates for active notes
    for (int i = mNoteHead; i < mNoteCount; i++) {
        CircleNote& note       = mSection->circleNotes[i];
        Entity* hitCircle      = &notes[mFirstNote + note.notesIndex];
        Entity* approachCircle = &notes[mApproachOffset + note.notesIndex];
        
        float timeLeft = note.hitTime - songTime;

        // Activate hit and approach circles when within visible window
        if (timeLeft <= 0.75f && timeLeft >= -GameMode::MISS_WINDOW) {
            if (!hitCircle->isActive())      hitCircle->activate();
            if (!approachCircle->isActive()) approachCircle->activate();
        }

        // Handle approach circle shrinking
        if (approachCircle->isActive() && timeLeft >= 0.0f) {
            float currScale = 150.0f * (1.0f + 3.0f * (timeLeft / 0.75f));
            approachCircle->setScale({currScale, currScale});
        }

        // Handle key presses for the active note
        if (i == mNoteHead && hitCircle->isActive()) {
            bool keyPressed = input.lanesPressed[0] || input.lanesPressed[1] ||
                              input.lanesPressed[2] || input.lanesPressed[3];

            // F/G/J/K keys or mouse click
            if ((keyPressed || input.mouseClicked) && isMouseOverNote(hitCircle, input.mousePos)) {
                float timeDiff = songTime - note.hitTime;
                
                // Score and display hit status
                mScoreboard->scoreHit(timeDiff);
                mPrevHitStatus = (std::fabs(timeDiff) <= Scoreboard::PERFECT_WINDOW) ? PERFECT
                               : (std::fabs(timeDiff) <= Scoreboard::GOOD_WINDOW)    ? GOOD
                                                                                     : BAD;
                mStatusTimer = 0.2f;
                mLastHitPos = hitCircle->getPosition();

                // Deactivate the active circle
                hitCircle->deactivate();
                approachCircle->deactivate();
                mNoteHead++;
            }
        }

        // Auto-miss when note passes the hit window
        if (hitCircle->isActive() && songTime - note.hitTime > GameMode::MISS_WINDOW) {
            mScoreboard->scoreMiss();
            hitCircle->deactivate();
            approachCircle->deactivate();

            mLastHitPos = hitCircle->getPosition();
            mPrevHitStatus   = MISS;
            mStatusTimer     = 0.2f;
            mNoteHead++;
        }

        if (timeLeft > 0.75f) break;
    }
}

void CircleMode::render(Entity* notes)
{
    // Reverse order to render earlier notes first
    for (int i = mNoteCount - 1; i >= mNoteHead; i--) {
        CircleNote& note = mSection->circleNotes[i];
        notes[mFirstNote + note.notesIndex].render();
        notes[mApproachOffset + note.notesIndex].render();
    }
}

void CircleMode::drawUI()
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

void CircleMode::shutdown()
{
    if (mStatusTexturesLoaded) {
        for (int i = 0; i < 4; i++) UnloadTexture(mStatusTextures[i]);
        mStatusTexturesLoaded = false;
    }

    mSection    = nullptr;
    mScoreboard = nullptr;
    mNoteCount  = 0;
    mNoteHead   = 0;
}

bool CircleMode::isMouseOverNote(const Entity* note, Vector2 mousePos) const
{
    // Check if mouse is within the circle radius
    Vector2 notePos = note->getPosition();
    float radius    = note->getScale().x / 2.0f;
    return ((mousePos.x - notePos.x) * (mousePos.x - notePos.x) + 
            (mousePos.y - notePos.y) * (mousePos.y - notePos.y)) <= (radius * radius);
}