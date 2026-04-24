#include "EndScene.h"

EndScene::EndScene()                                       : Scene { {0.0f, 0.0f}, "#000000" } {}
EndScene::EndScene(Vector2 origin, const char *bgHexCode)  : Scene { origin, bgHexCode } {}

EndScene::~EndScene() { shutdown(); }

void EndScene::initialise()
{
    mGameState.nextSceneID = 2;

    // Retrieve statistics from the GameState
    mScore    = mGameState.scoreStats.score;
    mAccuracy = mGameState.scoreStats.accuracy;
    mMaxCombo = mGameState.scoreStats.maxCombo;
    mPassed   = mGameState.scoreStats.passed;

    mBgTexture = LoadTexture("assets/textures/jailbreak_bg.png");

    // mBgMusic = LoadMusicStream("assets/audio/results.ogg");
    // SetMusicVolume(mBgMusic, 0.5f);
    // PlayMusicStream(mBgMusic);
}

void EndScene::update(float deltaTime)
{
    // UpdateMusicStream(mBgMusic);

    if (IsKeyPressed(KEY_ENTER)) mGameState.nextSceneID = 0;  // Menu
    if (IsKeyPressed(KEY_R))     mGameState.nextSceneID = 1;  // Retry
}

const char* EndScene::getGrade() const
{
    if      (mAccuracy >= 0.98f) return "SS";
    else if (mAccuracy >= 0.95f) return "S";
    else if (mAccuracy >= 0.90f) return "A";
    else if (mAccuracy >= 0.80f) return "B";
    else if (mAccuracy >= 0.70f) return "C";
    else                         return "D";
}

void EndScene::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    // Background image
    Rectangle bgSrc = {
        0.0f, 0.0f,
        static_cast<float>(mBgTexture.width),
        static_cast<float>(mBgTexture.height)
    };
    Rectangle bgDst = {
        0.0f, 0.0f,
        static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT)
    };
    DrawTexturePro(mBgTexture, bgSrc, bgDst, {0.0f, 0.0f}, 0.0f, WHITE);

    // Transparent text overlay panel
    constexpr float MARGIN  = 80.0f;
    constexpr float PANEL_X = MARGIN;
    constexpr float PANEL_Y = MARGIN;
    constexpr float PANEL_W = SCREEN_WIDTH  - MARGIN * 2.0f;
    constexpr float PANEL_H = SCREEN_HEIGHT - MARGIN * 2.0f;

    DrawRectangle(
        80, 80,
        static_cast<int>(SCREEN_WIDTH - 160.0f),
        static_cast<int>(SCREEN_HEIGHT - 160.0f),
        { 0, 0, 0, 180 }
    );

    // Result display (pass/fail)
    const char* result = mPassed ? "CLEARED" : "FAILED";
    Color resultColor  = mPassed ? Color{100, 220, 100, 255}
                                 : Color{220, 80, 80, 255};
    DrawText(result, SCREEN_WIDTH / 2 - MeasureText(result, 64) / 2, 130, 64, resultColor);

    // Song title
    const char* title = "Jailbreak - Neru ft. Kagamine Rin";
    DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, 22) / 2, 215, 22, LIGHTGRAY);

    // Divider line
    DrawLineEx(
        { 140.0f, 260.0f },
        { SCREEN_WIDTH - 140.0f, 260.0f },
        1.0f, { 255, 255, 255, 80 }
    );

    // Accuracy grade
    const char* grade = getGrade();
    Color gradeColor;

    if      (std::string(grade) == "SS") gradeColor = { 255, 215, 0, 255 };
    else if (std::string(grade) == "S")  gradeColor = GOLD;
    else if (std::string(grade) == "A")  gradeColor = RED;
    else if (std::string(grade) == "B")  gradeColor = BLUE;
    else if (std::string(grade) == "C")  gradeColor = { 255, 180, 60, 255 };
    else                                 gradeColor = { 220, 80, 80, 255 };

    DrawText(grade, 200, 300, 160, gradeColor);

    // Statistics
    constexpr float STATS_X   = PANEL_X + PANEL_W / 2.0f - 60.0f;
    constexpr float STATS_Y   = 290.0f;
    constexpr float ROW_GAP   = 70.0f;
    constexpr int   LABEL_SZ  = 18;
    constexpr int   VALUE_SZ  = 36;

    // Score
    DrawText("SCORE", 660, 290, 18, LIGHTGRAY);
    std::string scoreStr = std::to_string(mScore);
    DrawText(scoreStr.c_str(), 660, 314, 36, WHITE);

    // Accuracy
    DrawText("ACCURACY", 660, 360, 18, LIGHTGRAY);
    char accBuf[16];
    snprintf(accBuf, sizeof(accBuf), "%.2f%%", mAccuracy * 100.0f);
    DrawText(accBuf, 660, 384, 36, WHITE);

    // Max combo
    DrawText("MAX COMBO", 660, 430, 18, LIGHTGRAY);
    std::string comboStr = std::to_string(mMaxCombo) + "x";
    DrawText(comboStr.c_str(), 660, 454, 36, WHITE);

    // Divider line
    DrawLineEx(
        { 140.0f, 570.0f },
        { SCREEN_WIDTH - 140.0f, 570.0f },
        1.0f, { 255, 255, 255, 80 }
    );

    // Key prompts
    const char* menuPrompt  = "ENTER  |  Back to Menu";
    const char* retryPrompt = "R  |  Retry";

    DrawText(retryPrompt, SCREEN_WIDTH / 2 - MeasureText(retryPrompt, 24) / 2, 
        610, 24, {255, 255, 255, 200});
    DrawText(menuPrompt, SCREEN_WIDTH / 2 - MeasureText(menuPrompt, 24) / 2,
        650, 24, {255, 255, 255, 200});

    EndDrawing();
}

void EndScene::shutdown()
{
    UnloadTexture(mBgTexture);
    // UnloadMusicStream(mBgMusic);
}