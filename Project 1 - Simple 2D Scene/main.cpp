/**
* Author: Alan Tong
* Assignment: Simple 2D Scene
* Date due: 2/14/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/**
 * This 2D scene is inspired by the game Outer Wilds, simulating a dynamic overhead map of
 * its solar system. In the center is the Sun, followed by the Hourglass Twins, Timber Hearth
 * (and its moon the Attlerock), Brittle Hollow (and its moon Hollow's Lantern), Giant's Deep, 
 * and Dark Bramble. The Interloper, the comet, follows an eliptical path, while the White Hole
 * Station drifts diagonally across the top-right of the screen.
 * 
 * On initialization, each object is assigned a random starting position in its orbit (save for
 * the Sun, which remains at the origin). The Sun slowly expands over time, before rapidly 
 * contracting and simulating a supernova. The scene ends by fading in a game-over screen, reading
 * "You Are Dead" (referencing Outer Wild's death screen).
 * 
 * This project used a few functionalities (mainly for aesthetics) that were not explicitly taught
 * in class. This includes DrawText(), Fade(), and music streaming. However, all core assignment
 * requisites are implemented using standard raylib functions.
 * 
 * THEME: Outer Wilds ~ Background, Space Theme (Soundtrack)
 * 
 * OBJECTS: Sun, Hourglass Twins, Timber Hearth, Attlerock, Brittle Hollow,
 *          Hollow's Lantern, Giant's Deep, Dark Bramble, Interloper, White Hole Station
 * 
 * TRANSFORMATIONS: Circular Orbit    - Hourglass Twins, Timber Hearth, Brittle Hollow,
 *                                      Giant's Deep, Dark Bramble
 *                  Eliptical Orbit   - Interloper
 *                  Relational Orbit  - Attlerock (orbiting Timber Hearth),
 *                                      Hollow's Lantern (orbiting Brittle Hollow)
 *                  Diagonal Drift    - White Hole Station
 *                  Rectangular Drift - Outer Wilds Ventures Logo
 *                  Rotation          - Hourglass Twins (simulating the reciprocal orbit),
 *                                      All other orbiting astral bodies
 *                  Scaling           - Sun (expansion/contraction)
 * 
 * EXTRA CREDIT: While the background does not explicitly change color, I hope the screen-fill by the
 *               supernova and the subsequent fade-in of the game-over screen counts instead. I also
 *               added background music!
 */

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Enums
enum AppStatus { TERMINATED, RUNNING };
enum SunStatus { EXPANDING, CONTRACTING };
enum Direction { UP, DOWN, LEFT, RIGHT };

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
               STATION_FP[]         = "assets/white-hole-station.png",
               VENTURES_FP[]        = "assets/outer-wilds-ventures.png";

// Global Variables
AppStatus gAppStatus = RUNNING;
SunStatus gSunStatus = EXPANDING;
Direction gDirection = UP;

float gPreviousTicks           = 0.0f,
      gTwinsOrbitLocation      = 0.0f,
      gHearthOrbitLocation     = 0.0f,
      gAttlerockOrbitLocation  = 0.0f,
      gHollowOrbitLocation     = 0.0f,
      gLanternOrbitLocation    = 0.0f,
      gGiantOrbitLocation      = 0.0f,
      gBrambleOrbitLocation    = 0.0f,
      gInterloperOrbitLocation = 0.0f;

float gTwinsAngle     = 0.0f,
      gSunAngle       = 0.0f,
      gHearthAngle    = 0.0f,
      gAttlerockAngle = 0.0f,
      gHollowAngle    = 0.0f,
      gLanternAngle   = 0.0f,
      gGiantAngle     = 0.0f,
      gBrambleAngle   = 0.0f,
      gStationAngle   = 0.0f;

float gRadius     = 0.1f,
      gFadeAlpha  = 0.0f,
      gShrinkRate = 0.005f;
bool  gFade       = false;

Vector2 gSunScale               = {  0.03f, 0.03f  },
        gTwinsScale             = { 0.007f, 0.007f },
        gTimberHearthScale      = { 0.006f, 0.006f },
        gAttlerockScale         = { 0.002f, 0.002f },
        gBrittleHollowScale     = { 0.007f, 0.007f },
        gHollowsLanternScale    = { 0.003f, 0.003f },
        gGiantsDeepScale        = { 0.015f, 0.015f },
        gDarkBrambleScale       = {  0.01f, 0.01f  },
        gInterloperScale        = { 0.004f, 0.004f },
        gStationScale           = { 0.008f, 0.008f },
        gVenturesScale          = { 0.035f, 0.035f },
        gStationPosition        = { ORIGIN.x + 550.0f, ORIGIN.y - 300.0f },
        gVenturesPosition       = { SCREEN_WIDTH - 50.0f, SCREEN_HEIGHT - 50.0f },
        gBgPosition             = ORIGIN,
        gSunPosition            = ORIGIN;

Vector2 gTwinsPosition,
        gTimberHearthPosition,
        gAttlerockPosition,
        gBrittleHollowPosition,
        gHollowsLanternPosition,
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
          gStationTexture,
          gVenturesTexture;

Music music;

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
    InitAudioDevice();

    // Load background music
    music = LoadMusicStream("assets/track-space.mp3");
    PlayMusicStream(music);

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
    gStationTexture = LoadTexture(STATION_FP);
    gVenturesTexture = LoadTexture(VENTURES_FP);

    // Randomize starting positions
    gTwinsOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;
    gHearthOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;
    gAttlerockOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;
    gHollowOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;
    gLanternOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;
    gGiantOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;
    gBrambleOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;
    gInterloperOrbitLocation = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926535f;

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    UpdateMusicStream(music);

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
    gInterloperOrbitLocation -= (ORBIT_SPEED / 10.0f) * deltaTime;
    gInterloperPosition.x = ORIGIN.x - 260.0f + cos(gInterloperOrbitLocation) * 340.0f;
    gInterloperPosition.y = ORIGIN.y + sin(gInterloperOrbitLocation) * 200.0f;

    // Relational orbits
    gAttlerockOrbitLocation -= (ORBIT_SPEED / sqrt(50.0f / 4)) * deltaTime;
    gAttlerockPosition.x = gTimberHearthPosition.x + cos(gAttlerockOrbitLocation) * 50.0f;
    gAttlerockPosition.y = gTimberHearthPosition.y + sin(gAttlerockOrbitLocation) * 50.0f;

    gLanternOrbitLocation -= (ORBIT_SPEED / sqrt(50.0f / 4)) * deltaTime;
    gHollowsLanternPosition.x = gBrittleHollowPosition.x + cos(gLanternOrbitLocation) * 50.0f;
    gHollowsLanternPosition.y = gBrittleHollowPosition.y + sin(gLanternOrbitLocation) * 50.0f;

    // Rotations
    gTwinsAngle += 5.0f * deltaTime;
    gSunAngle += 2.0f * deltaTime;
    gHearthAngle += 3.0f * deltaTime;
    gAttlerockAngle += 4.0f * deltaTime;
    gHollowAngle += 3.0f * deltaTime;
    gLanternAngle += 4.0f * deltaTime;
    gGiantAngle += 5.0f * deltaTime;
    gBrambleAngle += 2.0f * deltaTime;
    gStationAngle += 1.0f * deltaTime;

    // Scaling
    if (gSunStatus == EXPANDING) {
        gSunScale.x *= pow(1.01f, deltaTime);
        gSunScale.y *= pow(1.01f, deltaTime);

        if (gSunScale.x > 0.04f) {
            gSunStatus = CONTRACTING;
        }
    } else {
        if (gSunScale.x > 0.001f) {
            gShrinkRate += 0.005f * deltaTime;
            gSunScale.x -= gShrinkRate * deltaTime;
            gSunScale.y -= gShrinkRate * deltaTime;
        }
        // Supernova effect
        gRadius *= pow(2.0f, deltaTime);
        if (gRadius >= SCREEN_WIDTH) {
            gFade = true;
        }
    }

    // Diagonal drift
    gStationPosition.x += 2.0f * deltaTime;
    gStationPosition.y -= 1.0f * deltaTime;

    // Square drift (logo)
    if (gDirection == UP) {
        gVenturesPosition.y -= 5.0f * deltaTime;
        if (gVenturesPosition.y < SCREEN_HEIGHT - 70.0f) {
            gDirection = LEFT;
        }
    } else if (gDirection == DOWN) {
        gVenturesPosition.y += 5.0f * deltaTime;
        if (gVenturesPosition.y > SCREEN_HEIGHT - 50.0f) {
            gDirection = RIGHT;
        }
    } else if (gDirection == LEFT) {
        gVenturesPosition.x -= 5.0f * deltaTime;
        if (gVenturesPosition.x < SCREEN_WIDTH - 70.0f) {
            gDirection = DOWN;
        }
    } else if (gDirection == RIGHT) {
        gVenturesPosition.x += 5.0f * deltaTime;
        if (gVenturesPosition.x > SCREEN_WIDTH - 50.0f) {
            gDirection = UP;
        }
    }

    // Game Over fade-in
    if (gFade) {
        gFadeAlpha += 0.3f * deltaTime;
        if (gFadeAlpha > 1.0f) {
            gFadeAlpha = 1.0f;
        }
    }
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
              stationTextureArea = {
                  0.0f, 0.0f,
                  static_cast<float>(gStationTexture.width),
                  static_cast<float>(gStationTexture.height)
              },
              venturesTextureArea = {
                  0.0f, 0.0f,
                  static_cast<float>(gVenturesTexture.width),
                  static_cast<float>(gVenturesTexture.height)
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
              },
              stationDestinationArea = {
                  gStationPosition.x, gStationPosition.y,
                  static_cast<float>(gStationTexture.width)  * gStationScale.x,
                  static_cast<float>(gStationTexture.height) * gStationScale.y
              },
              venturesDestinationArea = {
                  gVenturesPosition.x, gVenturesPosition.y,
                  static_cast<float>(gVenturesTexture.width)  * gVenturesScale.x,
                  static_cast<float>(gVenturesTexture.height) * gVenturesScale.y
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
            },
            stationOrigin = {
                static_cast<float>(gStationTexture.width)  * gStationScale.x / 2.0f,
                static_cast<float>(gStationTexture.height) * gStationScale.y / 2.0f
            },
            venturesOrigin = {
                static_cast<float>(gVenturesTexture.width)  * gVenturesScale.x / 2.0f,
                static_cast<float>(gVenturesTexture.height) * gVenturesScale.y / 2.0f
            };

    // Draw all objects (in order of background to foreground)
    DrawTexturePro(gBgTexture, bgTextureArea, bgDestinationArea, bgOrigin, 0.0f, WHITE);
    DrawTexturePro(gTwinsTexture, twinsTextureArea, twinsDestinationArea, twinsOrigin, gTwinsAngle, WHITE);
    DrawTexturePro(gTimberHearthTexture, hearthTextureArea, hearthDestinationArea, heartOrigin, gHearthAngle, WHITE);
    DrawTexturePro(gAttlerockTexture, attlerockTextureArea, attlerockDestinationArea, attlerockOrigin, gAttlerockAngle, WHITE);
    DrawTexturePro(gBrittleHollowTexture, hollowTextureArea, hollowDestinationArea, hollowOrigin, gHollowAngle, WHITE);
    DrawTexturePro(gHollowsLanternTexture, lanternTextureArea, lanternDestinationArea, lanternOrigin, gLanternAngle, WHITE);
    DrawTexturePro(gGiantsDeepTexture, giantTextureArea, giantDestinationArea, giantOrigin, gGiantAngle, WHITE);
    DrawTexturePro(gDarkBrambleTexture, brambleTextureArea, brambleDestinationArea, brambleOrigin, gBrambleAngle, WHITE);
    DrawTexturePro(gStationTexture, stationTextureArea, stationDestinationArea, stationOrigin, gStationAngle, WHITE);
    DrawTexturePro(gInterloperTexture, interloperTextureArea, interloperDestinationArea, interloperOrigin, 0.0f, WHITE);
    DrawTexturePro(gSunTexture, sunTextureArea, sunDestinationArea, sunOrigin, gSunAngle, WHITE);
    DrawTexturePro(gVenturesTexture, venturesTextureArea, venturesDestinationArea, venturesOrigin, 0.0f, WHITE);

    DrawCircle(ORIGIN.x, ORIGIN.y, gRadius, RAYWHITE);
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, gFadeAlpha));
    DrawText("You Are Dead", ORIGIN.x - MeasureText("You Are Dead", 60) / 2, ORIGIN.y - 30, 60, Fade(WHITE, gFadeAlpha));

    EndDrawing();
}

void shutdown() 
{
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
    UnloadTexture(gStationTexture);
    
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