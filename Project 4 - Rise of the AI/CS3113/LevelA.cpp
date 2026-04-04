#include "LevelA.h"

LevelA::LevelA()                                      : Scene { {0.0f}, nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
    mGameState.nextSceneID = 0;

    // mGameState.bgm = LoadMusicStream("assets/game/04 - Silent Forest.wav");
    // SetMusicVolume(mGameState.bgm, 0.33f);
    // PlayMusicStream(gState.bgm);

    // mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");

    /*
        ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
        (unsigned int *) mLevelData, // grid data
        "assets/sheet.png",          // texture filepath
        TILE_DIMENSION,              // tile size
        17, 8,                       // texture cols & rows
        mOrigin                      // in-game origin
    );

    /*
        ----------- PROTAGONIST -----------
    */
    std::map<Direction, std::vector<int>> playerAnimationAtlas = {
        {RIGHT,  { 23, 24, 25, 26 }},
        {LEFT,   { 23, 24, 25, 26 }},
        {UP,     {        0       }},
        {DOWN,   {        0       }},
    };

    float sizeRatio  = 32.0f / 48.0f;

    // Assets from https://opengameart.org/content/a-platformer-in-the-forest
    mGameState.player = new Entity(
        {mOrigin.x - 700.0f, mOrigin.y - 250.0f}, // position
        {150.0f * sizeRatio, 150.0f},             // scale
        "assets/characters.png",                  // texture file address
        ATLAS,                                    // single image or atlas?
        { 4, 23 },                                // atlas dimensions
        playerAnimationAtlas,                     // actual atlas
        PLAYER                                    // entity type
    );

    mGameState.player->setJumpingPower(550.0f);
    mGameState.player->setColliderDimensions({
        mGameState.player->getScale().x / 3.5f,
        mGameState.player->getScale().y / 3.0f
    });
    mGameState.player->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
}

void LevelA::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   mGameState.player->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      nullptr,        // collidable entities
      0               // col. entity count
   );

   if (mGameState.player->getPosition().y > 800.0f) mGameState.nextSceneID = 1;
}

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.player->render();
   mGameState.map->render();
}

void LevelA::shutdown()
{
   delete mGameState.player;
   delete mGameState.map;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
}