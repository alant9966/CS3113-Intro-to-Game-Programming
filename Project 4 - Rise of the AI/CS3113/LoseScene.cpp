#include "LoseScene.h"

LoseScene::LoseScene()                                      : Scene { {0.0f}, nullptr   } {}
LoseScene::LoseScene(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LoseScene::~LoseScene() { shutdown(); }

void LoseScene::initialise()
{
    mGameState.nextSceneID = 0;
    // Asset from https://opengameart.org/content/the-war-over-a-melon-field-main-menu-background
    mBgTexture = LoadTexture("assets/menu_bg.png");

    if (gGlobalState.isPlayingBgm) {
        StopMusicStream(gGlobalState.bgm);
        UnloadMusicStream(gGlobalState.bgm);
        
        gGlobalState.bgm = {};
        gGlobalState.isPlayingBgm = false;
    }

    // Music from https://opengameart.org/content/titlemenu-screen-bgm
    mGameState.bgm = LoadMusicStream("assets/Dream.mp3");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);
}

void LoseScene::update(float deltaTime)
{
    if (IsKeyPressed(KEY_ENTER))
        mGameState.nextSceneID = -4; // Menu

    UpdateMusicStream(mGameState.bgm);
}

void LoseScene::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    Rectangle bgTextureArea = {
        0.0f, 0.0f,
        static_cast<float>(mBgTexture.width),
        static_cast<float>(mBgTexture.height) 
    };
    Rectangle bgDestinationArea = {
        720.0f, 450.0f,
        static_cast<float>(1440),
        static_cast<float>(900)
    };
    DrawTexturePro(
        mBgTexture,
        bgTextureArea,
        bgDestinationArea,
        { 720.0f, 450.0f },
        0.0f,
        WHITE
    );

    DrawText("YOU LOSE :(", 100, 250, 60, BLACK);
    DrawText("Press ENTER to Restart", 100, 350, 40, BLACK);
}

void LoseScene::shutdown()
{
    UnloadTexture(mBgTexture);

    UnloadMusicStream(mGameState.bgm);

    if (gGlobalState.isPlayingBgm) {
        StopMusicStream(gGlobalState.bgm);
        UnloadMusicStream(gGlobalState.bgm);
        
        gGlobalState.bgm = {};
        gGlobalState.isPlayingBgm = false;
    }
}