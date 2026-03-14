/**
* Author: Alan Tong
* Assignment: Lunar Lander
* Date due: 3/14/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/Entity.h"

// Enums
enum GameOverStatus { WIN, LOSE, PLAYING };

// Global Constants
constexpr int SCREEN_WIDTH  = 1440,
              SCREEN_HEIGHT = 900,
              FPS           = 120;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr char GRIS_BG[] = "assets/gris_bg.jpg";

                // in m/ms², since delta time is in ms
constexpr float ACCELERATION_OF_GRAVITY = 981.0f / 136.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f;

// Global Variables
AppStatus      gAppStatus      = RUNNING;
GameOverStatus gGameOverStatus = PLAYING;

float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f,
      gFuelAmount      = 100.0f;

bool gIsRight       = true;
bool gIsGameOver    = false;
bool gIsMovingRight = false;
bool gIsMovingLeft  = false;

Texture2D gBgTexture;
Music music;

Entity *gGris      = nullptr;
Entity *gHandBlock = nullptr;
Entity *gLogo      = nullptr;
Entity *gWind      = nullptr;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise() 
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Lander");
    InitAudioDevice();

    music = LoadMusicStream("assets/mae_st.mp3");
    PlayMusicStream(music);

    gBgTexture = LoadTexture(GRIS_BG);

    /*
        ----------- PROTAGONIST -----------
    */
    float sizeRatio = 174.0f / 375.0f;
    gGris = new Entity(
        { ORIGIN.x, ORIGIN.y - 200.0f },
        { 100.0f * sizeRatio, 100.0f },
        "assets/gris.png",
        PLAYER
    );
    gGris->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

    /*
        ----------- GUST OF WIND (MOVEMENT) -----------
    */
    std::map<Direction, std::vector<int>> animationAtlas = {
        {LEFT,   { 4, 5, 6 }},
        {RIGHT,  { 4, 5, 6 }},
    };

    gWind = new Entity(
        { ORIGIN.x, ORIGIN.y },
        { 50.0f, 50.0f },
        "assets/wind_gust.png",
        ATLAS,
        { 4, 3 },
        animationAtlas,
        WIND
    );
    gWind->setVisibility(false);
    gWind->setFrameSpeed(2);

    /*
        ----------- HAND (WIN CONDITION) -----------
    */
    gHandBlock = new Entity(
        { ORIGIN.x - 210.0f, ORIGIN.y + 230.0f },
        { 360.0f, 70.0f },
        "assets/dummy.png",
        BLOCK
    );
    gHandBlock->setVisibility(false);

    /*
        ----------- LOGO (SECRET WIN CONDITION) -----------
    */
    gLogo = new Entity(
        { SCREEN_WIDTH - 80.0f, SCREEN_HEIGHT - 80.0f },
        { 100.0f, 100.0f },
        "assets/gris_logo.png",
        BLOCK
    );
    gLogo->setDirection(LEFT);

    SetTargetFPS(FPS);
}

void processInput()
{
    if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && gFuelAmount > 0.0f) {
        gGris->accelRight();
        gIsMovingRight = true;
        if (gFuelAmount > 0.0f) gFuelAmount -= 0.2f;
    }
    else if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && gFuelAmount > 0.0f) {
        gGris->accelLeft();
        gIsMovingLeft = true;
        if (gFuelAmount > 0.0f) gFuelAmount -= 0.2f;
    }
    else {
        gGris->driftStop();
        gIsMovingRight = false;
        gIsMovingLeft = false;
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    UpdateMusicStream(music);

    if (gIsGameOver) return;

    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    Entity* collidableEntities[2] = {gHandBlock, gLogo};

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gGris->update(FIXED_TIMESTEP, collidableEntities, 2);

        if (gIsMovingRight || gIsMovingLeft) {
            gWind->setDirection(gGris->getDirection());
            
            if (gGris->getDirection() == RIGHT) {
                gWind->setPosition({ gGris->getPosition().x - 40.0f, gGris->getPosition().y - 15.0f });
            } else {
                gWind->setPosition({ gGris->getPosition().x + 40.0f, gGris->getPosition().y - 15.0f });
            }
            
            gWind->setVisibility(true);
            gWind->animate(deltaTime);
        } else {
            gWind->setVisibility(false);
        }

        deltaTime -= FIXED_TIMESTEP;
    }

    // Land on hand or logo = win!
    if (gGris->isCollidingBottom()) {
        gIsGameOver = true;
        gGameOverStatus = WIN;
    }
    // Fall off-screen = lose!
    else if (gGris->getPosition().y > SCREEN_HEIGHT) {
        gIsGameOver = true;
        gGameOverStatus = LOSE;
    }

    if (gLogo->getDirection() == LEFT) {
        if (gLogo->getPosition().x <= SCREEN_WIDTH - 200.0f) {
            gLogo->moveRight();
            return;
        }
        gLogo->moveLeft();
    } else {
        if (gLogo->getPosition().x >= SCREEN_WIDTH - 80.0f) {
            gLogo->moveLeft();
            return;
        }
        gLogo->moveRight();
    }
    gLogo->update(deltaTime, nullptr, 0);
}

void render()
{
    BeginDrawing();
    
    Rectangle bgTextureArea = {
        0.0f, 0.0f,
        static_cast<float>(gBgTexture.width),
        static_cast<float>(gBgTexture.height) 
    };
    Rectangle bgDestinationArea = {
        ORIGIN.x, ORIGIN.y,
        static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT)
    };
    DrawTexturePro(gBgTexture, bgTextureArea, bgDestinationArea, ORIGIN, 0.0f, WHITE);

    gGris->render();
    gHandBlock->render();
    gLogo->render();
    gWind->render();

    DrawText(TextFormat("Land on the statue's hand to win!"), 35, 20, 30, GRAY);
    
    if (gFuelAmount > 0.0f) {
        DrawText(TextFormat("Remaining Fuel: %.0f%%", gFuelAmount), 35, 60, 30, GRAY);
    } else {
        DrawText(TextFormat("Remaining Fuel: 0%%"), 35, 60, 30, GRAY);
    }

    // Game Over UI
    if (gIsGameOver) {
        // Fade out background
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(LIGHTGRAY, 0.7f));

        if (gGameOverStatus == WIN) {
            // Winning text!
            DrawText(TextFormat("Mission accomplished! Gris landed gracefully."), (SCREEN_WIDTH / 2) - 570,
                SCREEN_HEIGHT / 2 - 50, 50, BLACK);
        } else {
            // Losing text :(
            DrawText(TextFormat("Mission failed Gris missed the landing..."), (SCREEN_WIDTH / 2) - 525,
                SCREEN_HEIGHT / 2 - 50, 50, BLACK);
        }

        DrawText("Please exit the window to replay.", (SCREEN_WIDTH / 2) - 240,
            SCREEN_HEIGHT / 2 + 30, 30, GRAY);
    }

    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gBgTexture);
    UnloadMusicStream(music);
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
        render();
    }

    shutdown();

    return 0;
}
