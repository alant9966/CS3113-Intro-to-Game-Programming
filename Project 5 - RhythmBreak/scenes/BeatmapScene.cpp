#include "BeatmapScene.h"
#include <stdexcept>
#include <fstream> // temp
#include <iomanip> // temp
#include <cstdlib> // temp

BeatmapScene::BeatmapScene()                                      : Scene { {0.0f, 0.0f}, "#000000" } {}
BeatmapScene::BeatmapScene(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

BeatmapScene::~BeatmapScene() { shutdown(); }

// Initialise all note/lane entities and the beatmap clock
void BeatmapScene::initialise()
{
    mShader.load("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

    mScoreboard.reset();
    mBeatmap = Beatmap::loadJSON("assets/beatmap.json");

    // Initialise custom cursor texture
    mCursorTexture = LoadTexture("assets/textures/cursor.png");
    HideCursor();

    // Initialise music stream and beatmap clock
    Music bgm = LoadMusicStream(mBeatmap.audio.c_str());
    mClock.initialise(bgm, mBeatmap.offset);

    // Initialise hit sound effects
    mFallingHitSound = LoadSound("assets/audio/falling-key-press.wav");
    mCircleHitSound  = LoadSound("assets/audio/circle-hit.wav");

    // Set total notes count for accuracy calculation
    mScoreboard.setTotalNotes(mBeatmap.totalNotes());

    // Allocate notes for beatmap sections
    int circleNoteCount = 0;
    for (auto& section : mBeatmap.sections) {
        circleNoteCount += static_cast<int>(section.circleNotes.size());
    }
    mNumApproachNotes = circleNoteCount;
    mNumNotes = mBeatmap.totalNotes() + mNumApproachNotes;
    mNotes = new Entity[mNumNotes];

    // Pre-initialise all sections to load textures
    for (int i = 0; i < static_cast<int>(mBeatmap.sections.size()); i++) {
        switchSection(i);
    }

    // Initialise the first section (falling-note)
    switchSection(0);

    // Initialise the background video player
    mVideoPlayer.initialise("assets/jailbreak_video.mpg");

    // Start music and beatmap clock
    mClock.play();
    mIsPlaying = true;

    mCurrSection = 0;
    mGameState.nextSceneID = 1;  // beatmap scene
}

void BeatmapScene::switchSection(int sectionIndex)
{
    if (mActiveMode) mActiveMode->shutdown();

    mCurrSection = sectionIndex;
    Section& section = mBeatmap.sections[sectionIndex];

    // Calculate the index of the first note for the new section
    int firstNote = 0;
    for (int i = 0; i < sectionIndex; i++) {
        firstNote += static_cast<int>(mBeatmap.sections[i].fallingNotes.size());
        firstNote += static_cast<int>(mBeatmap.sections[i].circleNotes.size());
    }

    // Initialise the new mode based on the section
    int approachBase = mBeatmap.totalNotes();
    switch (section.noteMode) {
    case FALLING:
        mFallingMode.initialise(&section, mNotes, &mScoreboard, firstNote);
        mActiveMode = &mFallingMode;
        break;
    case CIRCLE:
        // Determine the number of circle notes before this section
        for (int i = 0; i < sectionIndex; i++) {
            approachBase += static_cast<int>(mBeatmap.sections[i].circleNotes.size());
        }
        mCircleMode.initialise(&section, mNotes, &mScoreboard, firstNote, approachBase);
        mActiveMode = &mCircleMode;
        break;
    case SCANLINE:
        mScanlineMode.initialise(&section, mNotes, &mScoreboard, firstNote);
        mActiveMode = &mScanlineMode;
        break;
    default:
        break;
    }
}

// Update the active mode and check for beatmap completion
void BeatmapScene::update(float deltaTime)
{
    if (!mIsPlaying) return;

    // Shader is only active for circle mode sections
    mShaderActive = (mBeatmap.sections[mCurrSection].noteMode == CIRCLE);
 
    // Update the music stream and clock
    mClock.update();
    float songTime = mClock.getSongTime();

    // Update the video stream
    mVideoPlayer.update(songTime);

    // Capture keyboard and mouse input
    Input input;
    input.lanesPressed[0] = IsKeyPressed(KEY_D);
    input.lanesPressed[1] = IsKeyPressed(KEY_F);
    input.lanesPressed[2] = IsKeyPressed(KEY_J);
    input.lanesPressed[3] = IsKeyPressed(KEY_K);
    input.mousePos        = GetMousePosition();
    input.mouseClicked    = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    // Update the active mode (note positions, hit/miss detection)
    mActiveMode->update(mNotes, input, songTime, deltaTime);

    // Play hit sound effect for perfect hits
    int currHitCount = mScoreboard.getHitCount();
    if (currHitCount != mLastSoundHitCount) {
        if (mScoreboard.getPrevStatus() == PERFECT) {
            if (mBeatmap.sections[mCurrSection].noteMode == CIRCLE || 
                mBeatmap.sections[mCurrSection].noteMode == SCANLINE) {
                PlaySound(mCircleHitSound);
            } else {
                PlaySound(mFallingHitSound);
            }
        }
        mLastSoundHitCount = currHitCount;
    }
    
    // Check if a section switch is needed
    if (mCurrSection + 1 < static_cast<int>(mBeatmap.sections.size())) {
        if (songTime >= mBeatmap.sections[mCurrSection].endTime) {
            switchSection(mCurrSection + 1);
        }
    }

    // Fade out the song at 2 minutes
    if (!mFading && songTime >= 125.0f) {
        mFading    = true;
        mFadeTimer = 0.0f;
    }

    // Volume fade out and end the beatmap
    if (mFading) {
        mFadeTimer += deltaTime;

        float volume = 1.0f - (mFadeTimer / 2.0f);
        SetMusicVolume(mClock.getBgm(), std::max(volume, 0.0f));

        if (mFadeTimer >= 2.0f) {
            mGameState.scoreStats.score    = mScoreboard.getScore();
            mGameState.scoreStats.accuracy = mScoreboard.getAccuracy();
            mGameState.scoreStats.maxCombo = mScoreboard.getMaxCombo();
            mGameState.scoreStats.passed   = mScoreboard.getAccuracy() >= 0.6f;
    
            mGameState.nextSceneID = 2;  // results scene
        }
    }
}

// Render the active mode (notes/VFX) and UI elements
void BeatmapScene::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    // Update shader light position to the cursor position
    if (mShaderActive) {
        mShader.setVector2("lightPosition", GetMousePosition());
        mShader.begin();
    }

    // Render background video
    mVideoPlayer.render();

    // Render notes and effects for the active mode
    mActiveMode->render(mNotes);

    if (mShaderActive) {
        mShader.end();
    }

    // Render the active mode's UI
    mActiveMode->drawUI();

    // ------------------------ Global UI ------------------------

    // Score
    std::string score = std::to_string(mScoreboard.getScore());
    DrawText(score.c_str(), 1440 - MeasureText(score.c_str(), 24) - 20, 16, 24, WHITE);

    // Combo
    std::string combo = std::to_string(mScoreboard.getCombo());
    if (!combo.empty()) {
        DrawText(combo.c_str(), 720 - MeasureText(combo.c_str(), 20) / 2, 16, 20, GOLD);
    }

    // Accuracy
    char accBuffer[16];
    snprintf(accBuffer, sizeof(accBuffer), "%.1f%%", mScoreboard.getAccuracy() * 100.0f);
    DrawText(accBuffer, 20, 16, 20, LIGHTGRAY);

    // Cursor
    Vector2 mousePos = GetMousePosition();
    Rectangle src = {
        0.0f, 0.0f,
        static_cast<float>(mCursorTexture.width),
        static_cast<float>(mCursorTexture.height)
    };
    Rectangle dst = {
        mousePos.x - mCursorTexture.width / 2.0f,
        mousePos.y - mCursorTexture.height / 2.0f,
        static_cast<float>(mCursorTexture.width) * 0.7f,
        static_cast<float>(mCursorTexture.height) * 0.7f
    };
    DrawTexturePro(mCursorTexture, src, dst, {0.0f, 0.0f}, 0.0f, WHITE);

    EndDrawing();
}

// Unload resources and reset state
void BeatmapScene::shutdown()
{
    UnloadMusicStream(mClock.getBgm());

    UnloadSound(mFallingHitSound);
    UnloadSound(mCircleHitSound);

    UnloadTexture(mCursorTexture);

    if (mActiveMode) mActiveMode->shutdown();

    delete[] mNotes;
    mNotes = nullptr;

    mNumNotes    = 0;
    mCurrSection = 0;
    mIsPlaying   = false;
    mActiveMode  = nullptr;
}