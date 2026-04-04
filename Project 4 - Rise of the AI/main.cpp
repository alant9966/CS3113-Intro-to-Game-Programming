#include "CS3113/LevelA.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1440,
              SCREEN_HEIGHT    = 900,
              FPS              = 120,
              NUMBER_OF_LEVELS = 3;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Camera2D gCamera = { 0 };

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

LevelA *gLevelA = nullptr;
// LevelB *gLevelB = nullptr;
// LevelC *gLevelC = nullptr;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{
    gCurrentScene = scene;
    gCurrentScene->initialise();
    gCamera.target = gCurrentScene->getState().player->getPosition();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    gLevelA = new LevelA(ORIGIN, "#C0897E");
    // gLevelB = new LevelB(ORIGIN, "#011627");
    // gLevelC = new LevelC(ORIGIN, "#FFFFFF");

    gLevels.push_back(gLevelA);
    // gLevels.push_back(gLevelB);
    // gLevels.push_back(gLevelC);

    switchToScene(gLevels[0]);

    gCamera.target = { gCurrentScene->getState().map->getLeftBoundary() + (GetScreenWidth() / 2), 
                       gCurrentScene->getState().map->getTopBoundary()  + (GetScreenHeight() / 2) };
    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    SetTargetFPS(FPS);
}

void processInput() 
{
    gCurrentScene->getState().player->resetMovement();

    if      (IsKeyDown(KEY_A)) gCurrentScene->getState().player->moveLeft();
    else if (IsKeyDown(KEY_D)) gCurrentScene->getState().player->moveRight();

    if (IsKeyPressed(KEY_W) && 
        gCurrentScene->getState().player->isCollidingBottom())
    {
        gCurrentScene->getState().player->jump();
        // PlaySound(gCurrentScene->getState().jumpSound);
    }

    if (GetLength(gCurrentScene->getState().player->getMovement()) > 1.0f) 
        gCurrentScene->getState().player->normaliseMovement();

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;

        float currentPlayerX = gCurrentScene->getState().player->getPosition().x;
        Vector2 currentPlayerPosition = { currentPlayerX, ORIGIN.y };

        if (currentPlayerX > gCurrentScene->getState().map->getLeftBoundary()  + (GetScreenWidth() / 2) &&
            currentPlayerX < gCurrentScene->getState().map->getRightBoundary() - (GetScreenWidth() / 2))
            panCamera(&gCamera, &currentPlayerPosition);
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);

    gCurrentScene->render();

    EndMode2D();
    EndDrawing();
}

void shutdown() 
{
    delete gLevelA;
    // delete gLevelB;
    // delete gLevelC;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        if (gCurrentScene->getState().nextSceneID > 0)
        {
            int id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}