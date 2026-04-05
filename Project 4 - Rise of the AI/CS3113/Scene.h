#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

struct GlobalState
{
    int lives = 5;
    int level = 1;

    Music bgm = {};
    bool isPlayingBgm = false;
};

struct GameState
{
    Entity *player   = nullptr;
    Entity **enemies = nullptr;
    Map    *map      = nullptr;

    Music bgm       = {};
    Sound walkSound = {};
    Sound jumpSound = {};
    Sound hitSound  = {};

    int nextSceneID = 0;
};

class Scene 
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";
    
public:
    static GlobalState gGlobalState;

    Scene();
    Scene(Vector2 origin, const char *bgHexCode);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    
    GameState   getState()           const { return mGameState; }
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif