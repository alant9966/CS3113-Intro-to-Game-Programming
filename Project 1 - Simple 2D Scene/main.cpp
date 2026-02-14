/**
* Author: Alan Tong
* Assignment: Simple 2D Scene
* Date due: 2/14/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// OBJECTS: sun, twins, timber hearth (+attlerock), brittle hollow (+lantern),
//       giant's deep, dark bramble, (maybe) interloper
// ROTATION: ash/ember twin rotation (one sprite)
// SCALING: sun expanding - gradually expand for x seconds, then quickly contract and supernova (maybe) - or have it "shake" when approaching maximum capacity
    // restart game state after supernova (fade in from white)
// RELATIVE POSITION: attlerock and hollow's lantern orbitting around planet
// BACKGROUND: space, main music theme

// maybe - wandering quantum moon?
// need a second translation pattern (not orbitting) -> maybe deep space satellite going up and down bc top down view, slowing down at ends

// Enums
enum AppStatus { TERMINATED, RUNNING };

// Global Constants
constexpr int SCREEN_WIDTH  = 1440,
              SCREEN_HEIGHT = 900,
              FPS           = 60;

constexpr float ORBIT_SPEED = 0.5f;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr char OUTER_WILDS_BG[]     = "assets/ow_space.png",
               SUN_FP[]             = "assets/sun.png",
               TWINS_FP[]           = "assets/hourglass-twins.png",
               TIMBER_HEARTH_FP[]   = "assets/timber-hearth.png",
               ATTLEROCK_FP[]       = "assets/attlerock.png",
               BRITTLE_HOLLOW_FP[]  = "assets/brittle-hollow.png",
               HOLLOWS_LANTERN_FP[] = "assets/hollows-lantern.png",
               GIANTS_DEEP_FP[]     = "assets/giants-deep.png",
               DARK_BRAMBLE_FP[]    = "assets/dark-bramble.png",
               INTERLOPER_FP[]      = "assets/interloper.png",
               QUANTUM_MOON_FP[]    = "assets/quantum-moon.png";

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks           = 0.0f,
      gTwinsOrbitLocation      = 0.0f,
      gHearthOrbitLocation     = 0.0f,
      gHollowOrbitLocation     = 0.0f,
      gGiantOrbitLocation      = 0.0f,
      gBrambleOrbitLocation    = 0.0f,
      gInterloperOrbitLocation = 0.0f;

Vector2 gBgPosition             = ORIGIN,
        gSunPosition            = ORIGIN,
        gSunScale               = { 0.03f, 0.03f },
        gTwinsScale             = { 0.007f, 0.007f },
        gTimberHearthScale      = { 0.006f, 0.006f },
        gAttlerockPosition      = { ORIGIN.x + 230.0f, ORIGIN.y }, // TODO delete
        gAttlerockScale         = { 0.002f, 0.002f },
        gBrittleHollowScale     = { 0.007f, 0.007f },
        gHollowsLanternPosition = { ORIGIN.x + 330.0f, ORIGIN.y }, // TODO delete
        gHollowsLanternScale    = { 0.003f, 0.003f },
        gGiantsDeepScale        = { 0.015f, 0.015f },
        gDarkBrambleScale       = { 0.01f, 0.01f },
        
        gInterloperScale        = { 0.004f, 0.004f },
        gQuantumMoonPosition    = { ORIGIN.x, ORIGIN.y },
        gQuantumMoonScale       = { 1.0f, 1.0f };

Vector2 gTwinsPosition,
        gTimberHearthPosition,
        // gAttlerockPosition,
        gBrittleHollowPosition,
        // gHollowsLanternPosition,
        gGiantsDeepPosition,
        gDarkBramblePosition,
        gInterloperPosition;

Texture2D gBgTexture,
          gSunTexture,
          gTwinsTexture,
          gTimberHearthTexture,
          gAttlerockTexture,
          gBrittleHollowTexture,
          gHollowsLanternTexture,
          gGiantsDeepTexture,
          gDarkBrambleTexture,
          gInterloperTexture,
          gQuantumMoonTexture;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple 2D Scene");

    // Load all necessary textures
    gBgTexture = LoadTexture(OUTER_WILDS_BG);
    gSunTexture = LoadTexture(SUN_FP);
    gTwinsTexture = LoadTexture(TWINS_FP);
    gTimberHearthTexture = LoadTexture(TIMBER_HEARTH_FP);
    gAttlerockTexture = LoadTexture(ATTLEROCK_FP);
    gBrittleHollowTexture = LoadTexture(BRITTLE_HOLLOW_FP);
    gHollowsLanternTexture = LoadTexture(HOLLOWS_LANTERN_FP);
    gGiantsDeepTexture = LoadTexture(GIANTS_DEEP_FP);
    gDarkBrambleTexture = LoadTexture(DARK_BRAMBLE_FP);
    gInterloperTexture = LoadTexture(INTERLOPER_FP);
    gQuantumMoonTexture = LoadTexture(QUANTUM_MOON_FP);

    // Randomize starting positions
    gTwinsOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;
    gHearthOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;
    gHollowOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;
    gGiantOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;
    gBrambleOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;
    gInterloperOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() {
    // Delta Time
    float ticks = (float) GetTime();          
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    // Fixed orbits
    gTwinsOrbitLocation -= (ORBIT_SPEED / sqrt(100.0f)) * deltaTime;
    gTwinsPosition.x = ORIGIN.x + cos(gTwinsOrbitLocation) * 100.0f;
    gTwinsPosition.y = ORIGIN.y + sin(gTwinsOrbitLocation) * 100.0f;

    gHearthOrbitLocation -= (ORBIT_SPEED / sqrt(200.0f)) * deltaTime;
    gTimberHearthPosition.x = ORIGIN.x + cos(gHearthOrbitLocation) * 200.0f;
    gTimberHearthPosition.y = ORIGIN.y + sin(gHearthOrbitLocation) * 200.0f;

    gHollowOrbitLocation -= (ORBIT_SPEED / sqrt(300.0f)) * deltaTime;
    gBrittleHollowPosition.x = ORIGIN.x + cos(gHollowOrbitLocation) * 300.0f;
    gBrittleHollowPosition.y = ORIGIN.y + sin(gHollowOrbitLocation) * 300.0f;

    gGiantOrbitLocation -= (ORBIT_SPEED / sqrt(420.0f)) * deltaTime;
    gGiantsDeepPosition.x = ORIGIN.x + cos(gGiantOrbitLocation) * 420.0f;
    gGiantsDeepPosition.y = ORIGIN.y + sin(gGiantOrbitLocation) * 420.0f;

    gBrambleOrbitLocation -= (ORBIT_SPEED / sqrt(540.0f)) * deltaTime;
    gDarkBramblePosition.x = ORIGIN.x + cos(gBrambleOrbitLocation) * 540.0f;
    gDarkBramblePosition.y = ORIGIN.y + sin(gBrambleOrbitLocation) * 540.0f;

    // Eliptical orbit (for Interloper)
    gInterloperOrbitLocation -= (ORBIT_SPEED / 10.0) * deltaTime;
    gInterloperPosition.x = ORIGIN.x - 260.0f + cos(gInterloperOrbitLocation) * 340.0f;
    gInterloperPosition.y = ORIGIN.y + sin(gInterloperOrbitLocation) * 200.0f;

    // Rotations


    // TODO if interloper or ash twin collides with circle hitbox inside sun,
    // dissapear it
    // also maybe TODO fix interloper starting at random point in orbit

}

void render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    // Texture UV coordinate rectangles
    Rectangle bgTextureArea = {
                  0.0f, 0.0f,
                  static_cast<float>(gBgTexture.width),
                  static_cast<float>(gBgTexture.height) 
              },
              sunTextureArea = { 
                  0.0f, 0.0f,
                  static_cast<float>(gSunTexture.width),
                  static_cast<float>(gSunTexture.height)
              },
              twinsTextureArea = {
                  0.0f, 0.0f,
                  static_cast<float>(gTwinsTexture.width),
                  static_cast<float>(gTwinsTexture.height)
              },
              hearthTextureArea = {
                  0.0f, 0.0f,
                  static_cast<float>(gTimberHearthTexture.width),
                  static_cast<float>(gTimberHearthTexture.height)
              },
              attlerockTextureArea = {
                  0.0f, 0.0f,
                  static_cast<float>(gAttlerockTexture.width),
                  static_cast<float>(gAttlerockTexture.height)
              },
              hollowTextureArea = {
                  0.0f, 0.0f,
                  static_cast<float>(gBrittleHollowTexture.width),
                  static_cast<float>(gBrittleHollowTexture.height)
              },
              lanternTextureArea = {
                  0.0f, 0.0f,
                  static_cast<float>(gHollowsLanternTexture.width),
                  static_cast<float>(gHollowsLanternTexture.height)
              },
              giantTextureArea = {
                  0.0f, 0.0f,
                  static_cast<float>(gGiantsDeepTexture.width),
                  static_cast<float>(gGiantsDeepTexture.height)
              },
              brambleTextureArea = {
                  0.0f, 0.0f,
                  static_cast<float>(gDarkBrambleTexture.width),
                  static_cast<float>(gDarkBrambleTexture.height)
              },
              interloperTextureArea = {
                  0.0f, 0.0f,
                  static_cast<float>(gInterloperTexture.width),
                  static_cast<float>(gInterloperTexture.height)
              },
              moonTextureArea = {
                  0.0f, 0.0f,
                  static_cast<float>(gQuantumMoonTexture.width),
                  static_cast<float>(gQuantumMoonTexture.height)
              };

    // Texture position rectangles
    Rectangle bgDestinationArea = {
                  gBgPosition.x, gBgPosition.y,
                  static_cast<float>(SCREEN_WIDTH),
                  static_cast<float>(SCREEN_HEIGHT)
              },
              sunDestinationArea = {
                  gSunPosition.x, gSunPosition.y,
                  static_cast<float>(gSunTexture.width)  * gSunScale.x,
                  static_cast<float>(gSunTexture.height) * gSunScale.y
              },
              twinsDestinationArea = {
                  gTwinsPosition.x, gTwinsPosition.y,
                  static_cast<float>(gTwinsTexture.width)  * gTwinsScale.x,
                  static_cast<float>(gTwinsTexture.height) * gTwinsScale.y
              },
              hearthDestinationArea = {
                  gTimberHearthPosition.x, gTimberHearthPosition.y,
                  static_cast<float>(gTimberHearthTexture.width)  * gTimberHearthScale.x,
                  static_cast<float>(gTimberHearthTexture.height) * gTimberHearthScale.y
              },
              attlerockDestinationArea = {
                  gAttlerockPosition.x, gAttlerockPosition.y,
                  static_cast<float>(gAttlerockTexture.width)  * gAttlerockScale.x,
                  static_cast<float>(gAttlerockTexture.height) * gAttlerockScale.y
              },
              hollowDestinationArea = {
                  gBrittleHollowPosition.x, gBrittleHollowPosition.y,
                  static_cast<float>(gBrittleHollowTexture.width)  * gBrittleHollowScale.x,
                  static_cast<float>(gBrittleHollowTexture.height) * gBrittleHollowScale.y
              },
              lanternDestinationArea = {
                  gHollowsLanternPosition.x, gHollowsLanternPosition.y,
                  static_cast<float>(gHollowsLanternTexture.width)  * gHollowsLanternScale.x,
                  static_cast<float>(gHollowsLanternTexture.height) * gHollowsLanternScale.y
              },
              giantDestinationArea = {
                  gGiantsDeepPosition.x, gGiantsDeepPosition.y,
                  static_cast<float>(gGiantsDeepTexture.width)  * gGiantsDeepScale.x,
                  static_cast<float>(gGiantsDeepTexture.height) * gGiantsDeepScale.y
              },
              brambleDestinationArea = {
                  gDarkBramblePosition.x, gDarkBramblePosition.y,
                  static_cast<float>(gDarkBrambleTexture.width)  * gDarkBrambleScale.x,
                  static_cast<float>(gDarkBrambleTexture.height) * gDarkBrambleScale.y
              },
              interloperDestinationArea = {
                  gInterloperPosition.x, gInterloperPosition.y,
                  static_cast<float>(gInterloperTexture.width)  * gInterloperScale.x,
                  static_cast<float>(gInterloperTexture.height) * gInterloperScale.y
              };
    
    // Texture origin offsets
    Vector2 bgOrigin = {
                static_cast<float>(SCREEN_WIDTH) / 2.0f,
                static_cast<float>(SCREEN_HEIGHT) / 2.0f
            },
            sunOrigin = {
                static_cast<float>(gSunTexture.width)  * gSunScale.x / 2.0f,
                static_cast<float>(gSunTexture.height) * gSunScale.y / 2.0f
            },
            twinsOrigin = {
                static_cast<float>(gTwinsTexture.width)  * gTwinsScale.x / 2.0f,
                static_cast<float>(gTwinsTexture.height) * gTwinsScale.y / 2.0f
            },
            heartOrigin = {
                static_cast<float>(gTimberHearthTexture.width)  * gTimberHearthScale.x / 2.0f,
                static_cast<float>(gTimberHearthTexture.height) * gTimberHearthScale.y / 2.0f
            },
            attlerockOrigin = {
                static_cast<float>(gAttlerockTexture.width)  * gAttlerockScale.x / 2.0f,
                static_cast<float>(gAttlerockTexture.height) * gAttlerockScale.y / 2.0f
            },
            hollowOrigin = {
                static_cast<float>(gBrittleHollowTexture.width)  * gBrittleHollowScale.x / 2.0f,
                static_cast<float>(gBrittleHollowTexture.height) * gBrittleHollowScale.y / 2.0f
            },
            lanternOrigin = {
                static_cast<float>(gHollowsLanternTexture.width)  * gHollowsLanternScale.x / 2.0f,
                static_cast<float>(gHollowsLanternTexture.height) * gHollowsLanternScale.y / 2.0f
            },
            giantOrigin = {
                static_cast<float>(gGiantsDeepTexture.width)  * gGiantsDeepScale.x / 2.0f,
                static_cast<float>(gGiantsDeepTexture.height) * gGiantsDeepScale.y / 2.0f
            },
            brambleOrigin = {
                static_cast<float>(gDarkBrambleTexture.width)  * gDarkBrambleScale.x / 2.0f,
                static_cast<float>(gDarkBrambleTexture.height) * gDarkBrambleScale.y / 2.0f
            },
            interloperOrigin = {
                static_cast<float>(gInterloperTexture.width)  * gInterloperScale.x / 2.0f,
                static_cast<float>(gInterloperTexture.height) * gInterloperScale.y / 2.0f
            };

    // Draw all objects (in order of background to foreground)
    DrawTexturePro(gBgTexture, bgTextureArea, bgDestinationArea, bgOrigin, 0.0f, WHITE);
    DrawTexturePro(gTwinsTexture, twinsTextureArea, twinsDestinationArea, twinsOrigin, 0.0f, WHITE);
    DrawTexturePro(gTimberHearthTexture, hearthTextureArea, hearthDestinationArea, heartOrigin, 0.0f, WHITE);
    DrawTexturePro(gAttlerockTexture, attlerockTextureArea, attlerockDestinationArea, attlerockOrigin, 0.0f, WHITE);
    DrawTexturePro(gBrittleHollowTexture, hollowTextureArea, hollowDestinationArea, hollowOrigin, 0.0f, WHITE);
    DrawTexturePro(gHollowsLanternTexture, lanternTextureArea, lanternDestinationArea, lanternOrigin, 0.0f, WHITE);
    DrawTexturePro(gGiantsDeepTexture, giantTextureArea, giantDestinationArea, giantOrigin, 0.0f, WHITE);
    DrawTexturePro(gDarkBrambleTexture, brambleTextureArea, brambleDestinationArea, brambleOrigin, 0.0f, WHITE);
    DrawTexturePro(gInterloperTexture, interloperTextureArea, interloperDestinationArea, interloperOrigin, 0.0f, WHITE);
    DrawTexturePro(gSunTexture, sunTextureArea, sunDestinationArea, sunOrigin, 0.0f, WHITE);

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow();
    UnloadTexture(gBgTexture);
    UnloadTexture(gSunTexture);
    UnloadTexture(gTwinsTexture);
    UnloadTexture(gTimberHearthTexture);
    UnloadTexture(gAttlerockTexture);
    UnloadTexture(gBrittleHollowTexture);
    UnloadTexture(gHollowsLanternTexture);
    UnloadTexture(gGiantsDeepTexture);
    UnloadTexture(gDarkBrambleTexture);
    UnloadTexture(gInterloperTexture);
    UnloadTexture(gQuantumMoonTexture);
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