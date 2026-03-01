/**
* Author: Alan Tong
* Assignment: Pong Variations
* Date due: 2/28/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1440,
              SCREEN_HEIGHT = 900,
              FPS           = 120;

constexpr float MARGIN = 100.0f;

constexpr Vector2 ORIGIN        = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  SCREEN_PARAMS = {     SCREEN_WIDTH, SCREEN_HEIGHT     };

constexpr char HOLLOW_KNIGHT_BG[] = "assets/hollow_knight_bg.png";

// Global Variables
AppStatus  gAppStatus  = RUNNING;
PlayStatus gPlayStatus = TWO_PLAYER;

float gPreviousTicks = 0.0f;
bool  gIsGameOver    = false;

int gNumBallsRendered = 1,
    gPlayer1Score     = 0,
    gPlayer2Score     = 0;

Texture2D gBgTexture;
Music music;

Entity* gKnight1 = nullptr;
Entity* gKnight2 = nullptr;
Entity* gBalls   = nullptr;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Variations");
    InitAudioDevice();

    music = LoadMusicStream("assets/city_of_tears.mp3");
    PlayMusicStream(music);

    gBgTexture = LoadTexture(HOLLOW_KNIGHT_BG);

    srand(time(NULL));

    /*
        ----------- KNIGHTS (PLAYERS) -----------
    */
    std::map<KnightStatus, std::vector<int>> animationAtlas = {
        {UP,   { 13, 14, 15, 16, 17, 18 }},
        {DOWN, { 19, 20, 21, 22, 23, 24 }},
        {IDLE, {            6           }},
    };

    gKnight1 = new Entity(
        { MARGIN, ORIGIN.y },         // Position
        { 100.0f, 100.0f },           // Scale
        "assets/knight_sprites.png",  // Texture File
        ATLAS,                        // Single Image / Atlas
        { 2, 13 },                    // Atlas Dimensions
        animationAtlas,               // Atlas
        KNIGHT                        // Entity Type
    );

    gKnight2 = new Entity(
        { SCREEN_WIDTH - MARGIN, ORIGIN.y },
        { 100.0f, 100.0f },        
        "assets/knight_sprites.png", 
        ATLAS,                      
        { 2, 13 },                 
        animationAtlas,              
        KNIGHT                        
    );
    gKnight2->setFlipped(true);

    gKnight1->setAttackTexture("assets/knight_attack.png");
    gKnight2->setAttackTexture("assets/knight_attack.png");

    /*
        ----------- BALLS -----------
    */    
    gBalls = new Entity[3];

    for (int i = 0; i < 3; i++) {
        gBalls[i].setTexture("assets/baldur_ball.png");
        gBalls[i].setEntityType(BALL);
        gBalls[i].setScale({35.0f, 35.0f});
        gBalls[i].setColliderDimensions({35.0f, 35.0f});
        gBalls[i].setPosition(ORIGIN);

        float rand_norm = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        gBalls[i].setMovement({cos(rand_norm * PI), sin(rand_norm * PI)});
    }

    //gAttackTexture = LoadTexture(ATTACK);
    SetTargetFPS(FPS);
}

void processInput() 
{
    gKnight1->resetMovement();
    gKnight2->resetMovement();

    if (IsKeyPressed(KEY_T)) {
        gPlayStatus = (gPlayStatus == SINGLE_PLAYER) ? TWO_PLAYER : SINGLE_PLAYER;

        if (gPlayStatus == SINGLE_PLAYER) {
            gKnight2->setSpeed(gKnight2->getSpeed() - 60); 
        } else {
            gKnight2->resetSpeed();
        }
    }
    
    // Knight 1 movement
    if      (IsKeyDown(KEY_W)) gKnight1->moveUp();
    else if (IsKeyDown(KEY_S)) gKnight1->moveDown();
    else    gKnight1->setKnightStatus(IDLE);

    if (gPlayStatus == TWO_PLAYER) {
        // Knight 2 movement
        if      (IsKeyDown(KEY_UP))   gKnight2->moveUp();
        else if (IsKeyDown(KEY_DOWN)) gKnight2->moveDown();
        else    gKnight2->setKnightStatus(IDLE);
    }

    // Set number of rendered balls
    int prev_balls = gNumBallsRendered;
    if (IsKeyPressed(KEY_ONE))   gNumBallsRendered = 1;
    if (IsKeyPressed(KEY_TWO))   gNumBallsRendered = 2;
    if (IsKeyPressed(KEY_THREE)) gNumBallsRendered = 3;

    if (prev_balls != gNumBallsRendered) {
        for (int i = 0; i < 3; i++)
            gBalls[i].resetPos(ORIGIN);
    }

    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{   
    UpdateMusicStream(music);

    // Game over - stop everything!
    if (gIsGameOver) return;

    float currentTicks = GetTime();
    float deltaTime = currentTicks - gPreviousTicks;
    gPreviousTicks = currentTicks;

    Entity* knightPtrs[2] = { gKnight1, gKnight2 };

    if (gPlayStatus == SINGLE_PLAYER) {
        // Track closest ball (straight-line distance)
        float minDist = -1.0f;
        float targetY = gBalls[0].getPosition().y;
        float targetX = gBalls[0].getPosition().x;

        for (int i = 0; i < gNumBallsRendered; i++) {
            float xDist = gBalls[i].getPosition().x - gKnight2->getPosition().x;
            float yDist = gBalls[i].getPosition().y - gKnight2->getPosition().y;

            if (minDist < 0.0f || pow(xDist, 2) + pow(yDist, 2) < minDist) {
                minDist = pow(xDist, 2) + pow(yDist, 2);
                targetY = gBalls[i].getPosition().y;
                targetX = gBalls[i].getPosition().x;
            }
        }

        // Simple vertical tracking (only if ball is in right half of screen)
        if (targetX > ORIGIN.x && gKnight2->getPosition().y < targetY - 20.0f) {
            gKnight2->moveDown();
        }
        else if (targetX > ORIGIN.x && gKnight2->getPosition().y > targetY + 20.0f) {
            gKnight2->moveUp();
        }
        else { gKnight2->setKnightStatus(IDLE); }
    }

    gKnight1->update(deltaTime, nullptr, 0, SCREEN_PARAMS);
    gKnight2->update(deltaTime, nullptr, 0, SCREEN_PARAMS);

    if (gNumBallsRendered == 1) {
        gBalls[0].update(deltaTime, knightPtrs, 2, SCREEN_PARAMS);
    } else if (gNumBallsRendered == 2) {
        gBalls[0].update(deltaTime, knightPtrs, 2, SCREEN_PARAMS);
        gBalls[1].update(deltaTime, knightPtrs, 2, SCREEN_PARAMS);
    } else {
        gBalls[0].update(deltaTime, knightPtrs, 2, SCREEN_PARAMS);
        gBalls[1].update(deltaTime, knightPtrs, 2, SCREEN_PARAMS);
        gBalls[2].update(deltaTime, knightPtrs, 2, SCREEN_PARAMS);
    }

    // Check for scoring balls
    for (int i = 0; i < gNumBallsRendered; i++) {
        // Left side - Player 2 scores
        if (gBalls[i].getPosition().x < 0) {
            gPlayer2Score++;
            gBalls[i].resetPos(ORIGIN);
        }
        else if (gBalls[i].getPosition().x > SCREEN_WIDTH) {
            gPlayer1Score++;
            gBalls[i].resetPos(ORIGIN);
        }
    }

    if (gPlayer1Score == 5 || gPlayer2Score == 5) {
        gIsGameOver = true;
    }
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

    gKnight1->render();
    gKnight2->render();

    if (gNumBallsRendered == 1) {
        gBalls[0].render();
    } else if (gNumBallsRendered == 2) {
        gBalls[0].render();
        gBalls[1].render();
    } else {
        gBalls[0].render();
        gBalls[1].render();
        gBalls[2].render();
    }

    DrawText(TextFormat("Score 5 points to win!"), SCREEN_WIDTH / 2 - 160, 20, 30, WHITE);
    DrawText(TextFormat("Player 1: %d", gPlayer1Score), SCREEN_WIDTH / 2 - 80, 60, 30, WHITE);
    DrawText(TextFormat("Player 2: %d", gPlayer2Score), SCREEN_WIDTH / 2 - 85, 100, 30, WHITE);

    // Game Over UI
    if (gIsGameOver) {
        // Fade out background
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(LIGHTGRAY, 0.7f));

        // Winning text!
        int winner = (gPlayer1Score == 5) ? 1 : 2;
        DrawText(TextFormat("Game over! Player %d wins!", winner), (SCREEN_WIDTH / 2) - 375,
            SCREEN_HEIGHT / 2 - 50, 60, WHITE);
        DrawText("Please exit the window to replay.", (SCREEN_WIDTH / 2) - 240,
            SCREEN_HEIGHT / 2 + 30, 30, LIGHTGRAY);
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