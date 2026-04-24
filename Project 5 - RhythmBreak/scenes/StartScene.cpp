#include "StartScene.h"

StartScene::StartScene()                                       : Scene { {0.0f, 0.0f}, "#000000" } {}
StartScene::StartScene(Vector2 origin, const char *bgHexCode)  : Scene { origin, bgHexCode } {}

StartScene::~StartScene() { shutdown(); }

void StartScene::initialise()
{
    mGameState.nextSceneID = 0;

    mBgTexture = LoadTexture("assets/textures/jailbreak_bg.png");

    // mBgMusic = LoadMusicStream("assets/audio/jailbreak_instr.ogg");
    // SetMusicVolume(mBgMusic, 0.5f);
    // PlayMusicStream(mBgMusic);
}

void StartScene::update(float deltaTime)
{
    // UpdateMusicStream(mBgMusic);

    if (IsKeyPressed(KEY_ENTER)) mGameState.nextSceneID = 1;
}

void StartScene::render()
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
    DrawRectangle(
        80, 80,
        static_cast<int>(SCREEN_WIDTH - 160.0f),
        static_cast<int>(SCREEN_HEIGHT - 160.0f),
        { 0, 0, 0, 180 }
    );

    // Song title
    const char* title    = "Jailbreak";
    const char* subtitle = "Neru ft. Kagamine Rin";

    DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, 52) / 2, 140, 52, WHITE);
    DrawText(subtitle, SCREEN_WIDTH / 2 - MeasureText(subtitle, 26) / 2, 208, 26, LIGHTGRAY);

    // Divider line
    DrawLineEx(
        { 140.0f, 260.0f },
        { SCREEN_WIDTH - 140.0f, 260.0f },
        1.0f, { 255, 255, 255, 80 }
    );

    // Instructions

    // Falling Mode
    DrawText("FALLING NOTES", 160, 300, 20, GOLD);
    DrawText("D / F / J / K", 160, 336, 18, WHITE);
    DrawText("Hit notes when they reach the line.", 160, 360, 18, LIGHTGRAY);

    // Circle Mode
    DrawText("CIRCLE MODE", 720, 300, 20, GOLD);
    DrawText("Mouse + any key (D/F/J/K)", 720, 336, 18, WHITE);
    DrawText("Click circles as they appear.", 720, 360, 18, LIGHTGRAY);

    // Scanline Mode
    DrawText("SCANLINE MODE", 160, 504, 20, GOLD);
    DrawText("Any key (D/F/J/K)", 160, 540, 18,  WHITE);
    DrawText("Press when the scanline passes a circle.", 160, 576, 18, LIGHTGRAY);

    // Press ENTER prompt
    const char* prompt = "Press ENTER to Play";
    DrawText(prompt, SCREEN_WIDTH / 2 - MeasureText(prompt, 28) / 2, 660, 28, GOLD);

    EndDrawing();
}

void StartScene::shutdown()
{
    UnloadTexture(mBgTexture);
    // UnloadMusicStream(mBgMusic);
}