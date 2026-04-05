#include "LevelB.h"

LevelB::LevelB()                                      : Scene { {0.0f}, nullptr   } {}
LevelB::LevelB(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelB::~LevelB() { shutdown(); }

void LevelB::initialise()
{
    mGameState.nextSceneID = 0;

    // Asset from https://opengameart.org/content/walking-on-snow-sound
    mGameState.walkSound = LoadSound("assets/SnowWalk.ogg");
    SetSoundVolume(mGameState.walkSound, 0.33f);

    // Asset from https://opengameart.org/content/jump-sound-effect-yo-frankie
    mGameState.jumpSound = LoadSound("assets/sfx_jump.mp3");
    
    // Asset from https://opengameart.org/content/player-hit-damage
    mGameState.hitSound  = LoadSound("assets/playerhit.mp3");

    /*
        ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL_WIDTH_B, LEVEL_HEIGHT_B, // map grid cols & rows
        (unsigned int *) mLevelData,   // grid data
        "assets/sheet.png",            // texture filepath
        TILE_DIMENSION,                // tile size
        17, 8,                         // texture cols & rows
        mOrigin                        // in-game origin
    );

    /*
        ----------- PROTAGONIST -----------
    */
    std::map<Direction, std::vector<int>> playerAnimationAtlas = {
        {RIGHT,  { 23, 24, 25, 26 }},
        {LEFT,   { 23, 24, 25, 26 }},
        {UP,     { }},
        {DOWN,   { }},
    };

    // Assets from https://opengameart.org/content/a-platformer-in-the-forest
    mGameState.player = new Entity(
        { mGameState.map->getLeftBoundary() + (TILE_DIMENSION * 3.0f),
          mGameState.map->getTopBoundary()  + (23 * TILE_DIMENSION) - (50.0f / 2.0f) }, 
        { 100.0f, 100.0f },
        "assets/characters.png",
        ATLAS,   
        { 1, 23 },
        playerAnimationAtlas,  
        PLAYER                                    
    );

    mGameState.player->setJumpingPower(620.0f);
    mGameState.player->setColliderDimensions({
        mGameState.player->getScale().x / 2.2f,
        mGameState.player->getScale().y / 1.3f
    });
    mGameState.player->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.player->setFrameSpeed(6);

    /*
        ----------- ENEMIES -----------
    */
    std::map<Direction, std::vector<int>> batAnimationAtlas = {
        {RIGHT,  { 0, 1, 2, 3 }},
        {LEFT,   { 0, 1, 2, 3 }},
        {UP,     { 0, 1, 2, 3 }},
        {DOWN,   { 0, 1, 2, 3 }},
    };

    mGameState.enemies = new Entity*[3];

    // Assets from https://admurin.itch.io/enemy-galore-1 
    mGameState.enemies[0] = new Entity(
        { mGameState.map->getLeftBoundary() + (TILE_DIMENSION * 3.0f),
          mGameState.map->getTopBoundary()  + (17 * TILE_DIMENSION) - (50.0 / 2.0f) },
        { 200.0f, 100.0f },
        "assets/bat_fly.png",
        ATLAS,
        { 1, 4 },
        batAnimationAtlas,
        NPC
    );

    mGameState.enemies[1] = new Entity(
        { mGameState.map->getLeftBoundary() + (14 * TILE_DIMENSION) + (TILE_DIMENSION / 2.0f),
          mGameState.map->getTopBoundary()  + (11 * TILE_DIMENSION) - (50.0 / 2.0f) },
        { 200.0f, 100.0f },
        "assets/bat_fly.png",
        ATLAS,
        { 1, 4 },
        batAnimationAtlas,
        NPC
    );

    mGameState.enemies[2] = new Entity(
        { mGameState.map->getLeftBoundary() + (6 * TILE_DIMENSION) + (TILE_DIMENSION / 2.0f),
          mGameState.map->getTopBoundary()  + (4 * TILE_DIMENSION) - (50.0 / 2.0f) },
        { 200.0f, 100.0f },
        "assets/bat_fly.png",
        ATLAS,
        { 1, 4 },
        batAnimationAtlas,
        NPC
    );

    for (int i = 0; i < 3; i++)
    {
        mGameState.enemies[i]->setAIType(FLYER);
        mGameState.enemies[i]->setSpeed(160);
        mGameState.enemies[i]->setFrameSpeed(6);
        mGameState.enemies[i]->setColliderDimensions({
            mGameState.enemies[i]->getScale().x / 3.0f,
            mGameState.enemies[i]->getScale().y / 2.0f
        });
        
        // Disable gravity for flying enemies
        mGameState.enemies[i]->setAcceleration({ 0.0f, 0.0f });
    }
}

void LevelB::update(float deltaTime)
{
    UpdateMusicStream(gGlobalState.bgm);

    mGameState.player->update(
        deltaTime,                                                         // delta time / fixed timestep
        nullptr,                                                           // player
        mGameState.map,                                                    // map
        mGameState.player->isInvincible() ? nullptr : mGameState.enemies,  // collidable entities
        mGameState.player->isInvincible() ? 0 : 3                          // col. entity count
    );

    for (int i = 0; i < 3; i++)
    {
        mGameState.enemies[i]->update(
            deltaTime,          
            mGameState.player,  
            mGameState.map,     
            nullptr,         
            0               
        );

        if (!mGameState.player->isInvincible() &&
            (mGameState.player->isColliding(mGameState.enemies[i]) ||
             mGameState.player->isCollidingEnemyBottom()))
        {
            PlaySound(mGameState.hitSound);
            gGlobalState.lives--;
            mGameState.player->startInvincibility();
        }
    }

    if (mGameState.map->isCollidingTile(mGameState.player->getPosition(), 32)) {
        gGlobalState.level = 3;
        mGameState.nextSceneID = 2; // C
    }

    if (gGlobalState.lives <= 0) {
        gGlobalState.lives = 5;
        gGlobalState.level = 1;
        mGameState.nextSceneID = -3; // Lose Scene
    }

    // Lose a life and respawn if player falls below map
    if (mGameState.player->getPosition().y > mGameState.map->getBottomBoundary()) {
        gGlobalState.lives--;
        mGameState.player->setPosition({
            mGameState.map->getLeftBoundary() + (TILE_DIMENSION * 3.0f),
            mGameState.map->getTopBoundary()  + (23 * TILE_DIMENSION) - (50.0f / 2.0f)
        });
        mGameState.player->startInvincibility();
    }

    if (IsKeyPressed(KEY_ONE)) {
        gGlobalState.level = 1;
        mGameState.nextSceneID = -1; // A
    } 
    else if (IsKeyPressed(KEY_TWO)) {
        gGlobalState.level = 2;
        mGameState.nextSceneID = 1; // B
    }
    else if (IsKeyPressed(KEY_THREE)) {
        gGlobalState.level = 3;
        mGameState.nextSceneID = 2; // C
    }
}

void LevelB::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mGameState.player->render();

    for (int i = 0; i < 3; i++) mGameState.enemies[i]->render();
}

void LevelB::shutdown()
{
    delete mGameState.player;
    
    for (int i = 0; i < 3; i++) delete mGameState.enemies[i];

    delete mGameState.map;

    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.walkSound);
    UnloadSound(mGameState.hitSound);
}