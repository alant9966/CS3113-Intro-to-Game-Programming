#include "Menu.h"

Menu::Menu()                                      : Scene { {0.0f}, nullptr   } {}
Menu::Menu(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Menu::~Menu() { shutdown(); }

void Menu::initialise()
{
    mGameState.nextSceneID = 0;
    // Asset from https://opengameart.org/content/the-war-over-a-melon-field-main-menu-background
    mBgTexture = LoadTexture("assets/menu_bg.png");

    // Music from https://opengameart.org/content/titlemenu-screen-bgm
    mGameState.bgm = LoadMusicStream("assets/Dream.mp3");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);
}

void Menu::update(float deltaTime)
{
    if (IsKeyPressed(KEY_ENTER))
        mGameState.nextSceneID = -1; // Level A

    UpdateMusicStream(mGameState.bgm);
}

void Menu::render()
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

    DrawText("A KING'S QUEST", 100, 250, 60, BLACK);
    DrawText("Press ENTER to Start", 100, 350, 40, BLACK);
}

void Menu::shutdown()
{
    UnloadTexture(mBgTexture);

    UnloadMusicStream(mGameState.bgm);
}