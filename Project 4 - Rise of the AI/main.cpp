#include "CS3113/LevelC.h"
#include "CS3113/LoseScene.h"

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

Menu      *gMenu      = nullptr;
WinScene  *gWinScene  = nullptr;
LoseScene *gLoseScene = nullptr;

LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
LevelC *gLevelC = nullptr;

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

    Entity *player = gCurrentScene->getState().player;
    if (player != nullptr) gCamera.target = player->getPosition();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    gMenu      = new Menu(ORIGIN, "#000000");
    gWinScene  = new WinScene(ORIGIN, "#000000");
    gLoseScene = new LoseScene(ORIGIN, "#000000");

    gLevelA = new LevelA(ORIGIN, "#C0897E");
    gLevelB = new LevelB(ORIGIN, "#C0897E");
    gLevelC = new LevelC(ORIGIN, "#C0897E");

    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);

    switchToScene(gMenu);

    if (gCurrentScene->getState().map != nullptr) {
        gCamera.target = { gCurrentScene->getState().map->getLeftBoundary() + (GetScreenWidth() / 2),
                           gCurrentScene->getState().map->getTopBoundary()  + (GetScreenHeight() / 2) };
    }
    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (gCurrentScene->getState().player != nullptr) {
        gCurrentScene->getState().player->resetMovement();

        if      (IsKeyDown(KEY_A)) gCurrentScene->getState().player->moveLeft();
        else if (IsKeyDown(KEY_D)) gCurrentScene->getState().player->moveRight();

        // Play walking sound when moving on platform
        if ((IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) &&
            gCurrentScene->getState().player->isCollidingBottomMap())
        {
            if (!IsSoundPlaying(gCurrentScene->getState().walkSound))
                PlaySound(gCurrentScene->getState().walkSound);
        } else {
            StopSound(gCurrentScene->getState().walkSound);
        }

        if (IsKeyPressed(KEY_W) && 
            gCurrentScene->getState().player->isCollidingBottomMap())
        {
            gCurrentScene->getState().player->jump();
            PlaySound(gCurrentScene->getState().jumpSound);
        }

        if (GetLength(gCurrentScene->getState().player->getMovement()) > 1.0f) 
            gCurrentScene->getState().player->normaliseMovement();
    }

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

        if (gCurrentScene->getState().player != nullptr &&
            gCurrentScene->getState().map != nullptr) 
        {
            Vector2 currentPlayerPosition = gCurrentScene->getState().player->getPosition();
            panCamera(&gCamera, &currentPlayerPosition);

            Map *map = gCurrentScene->getState().map;
            float mapWidth  = map->getRightBoundary() - map->getLeftBoundary();
            float mapHeight = map->getBottomBoundary() - map->getTopBoundary();

            // Limit horizontal scrolling
            if (mapWidth > GetScreenWidth())
                gCamera.target.x = fmax(map->getLeftBoundary() + (GetScreenWidth() / 2.0f),
                                        fmin(map->getRightBoundary() - (GetScreenWidth() / 2.0f),
                                             gCamera.target.x));
            else
                gCamera.target.x = map->getLeftBoundary() + (mapWidth / 2.0f); // Map center (x)

            // Limit vertical scrolling
            if (mapHeight > GetScreenHeight())
                gCamera.target.y = fmax(map->getTopBoundary() + (GetScreenHeight() / 2.0f),
                                        fmin(map->getBottomBoundary() - (GetScreenHeight() / 2.0f),
                                             gCamera.target.y));
            else
                gCamera.target.y = map->getTopBoundary() + (mapHeight / 2.0f); // Map center (y)
        }
    }
}

void render()
{
    BeginDrawing();

    if (gCurrentScene->getState().player != nullptr) {
        BeginMode2D(gCamera);
        gCurrentScene->render();
        EndMode2D();

        DrawText(TextFormat("Level %d", Scene::gGlobalState.level), 20, 20, 30, BLACK);
        DrawText(TextFormat("Lives Remaining: %d", Scene::gGlobalState.lives), 20, 65, 30, BLACK);
    } else {
        gCurrentScene->render();
    }
    
    EndDrawing();
}

void shutdown() 
{
    delete gLevelA;
    delete gLevelB;
    delete gLevelC;

    delete gMenu;
    delete gWinScene;
    delete gLoseScene;

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

        if (gCurrentScene->getState().nextSceneID != 0)
        {
            int sceneID = gCurrentScene->getState().nextSceneID;

            if      (sceneID == -1) switchToScene(gLevelA);
            else if (sceneID == -2) switchToScene(gWinScene);
            else if (sceneID == -3) switchToScene(gLoseScene);
            else if (sceneID == -4) switchToScene(gMenu);
            else                    switchToScene(gLevels[sceneID]);
        }

        render();
    }

    shutdown();

    return 0;
}