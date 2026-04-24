/**
* Author: Alan Tong
* Assignment: RhythmBreak
* Date due: [Your presentation date], 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "scenes/BeatmapScene.h"
#include "scenes/StartScene.h"
#include "scenes/EndScene.h"

// Global Constants
constexpr int FPS        = 120;
constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

// Global Variables
AppStatus gAppStatus = RUNNING;

Scene* gScenes[3]   = { nullptr, nullptr, nullptr };
Scene* gCurrScene   = nullptr;
int    gCurrSceneID = 0;  // start scene

// Function Declarations
void initialise();
void update();
void render();
void shutdown();
void switchToScene(int sceneID, GameState state = {});

void switchToScene(int sceneID, GameState state)
{
    if (gCurrScene) gCurrScene->shutdown();

    gCurrSceneID = sceneID;
    gCurrScene   = gScenes[sceneID];

    gCurrScene->getState() = state;
    gCurrScene->initialise();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RhythmBreak");
    
    SetWindowState(FLAG_WINDOW_TOPMOST);
    SetWindowFocused();
    ClearWindowState(FLAG_WINDOW_TOPMOST);

    InitAudioDevice();

    gScenes[0] = new StartScene();
    gScenes[1] = new BeatmapScene();
    gScenes[2] = new EndScene();

    SetTargetFPS(FPS);

    switchToScene(0);  // start scene (menu)
}

void update()
{
    if (WindowShouldClose()) {
        gAppStatus = TERMINATED;
        return;
    }

    float deltaTime = GetFrameTime();
    gCurrScene->update(deltaTime);

    // Check for scene transition
    if (gCurrScene->getState().nextSceneID != gCurrSceneID) {
        switchToScene(gCurrScene->getState().nextSceneID, gCurrScene->getState());
    }
}

void render()
{
    gCurrScene->render();
}

void shutdown()
{
    if (gCurrScene) gCurrScene->shutdown();

    for (int i = 0; i < 3; i++) {
        delete gScenes[i];
        gScenes[i] = nullptr;
    }

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        update();
        render();
    }

    shutdown();
    return 0;
}