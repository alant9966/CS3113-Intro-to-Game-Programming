#include "../modes/FallingMode.h"

#ifndef SCENE_H
#define SCENE_H

struct ScoreStats
{
    int   score    = 0;
    float accuracy = 0.0f;
    int   maxCombo = 0;
    bool  passed   = false;
};

struct GameState
{
    ScoreStats scoreStats;
    int        nextSceneID = 0;
};

class Scene 
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";
    
public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    
    GameState&       getState()                 { return mGameState;       } 
    const GameState& getState()           const { return mGameState;       }
    Vector2          getOrigin()          const { return mOrigin;          }
    const char*      getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif