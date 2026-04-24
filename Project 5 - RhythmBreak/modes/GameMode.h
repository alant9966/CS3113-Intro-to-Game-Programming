#include "../entities/Entity.h"

#ifndef GAMEMODE_H
#define GAMEMODE_H

// Stores input data
struct Input
{
    bool    lanesPressed[4] = {};  // D/F/J/K
    Vector2 mousePos        = {};
    bool    mouseClicked    = false;
};

// Inherited by FallingMode, CircleMode, and ScanlineMode
class GameMode
{
public:
    static constexpr float MISS_WINDOW = 0.2f;

    virtual ~GameMode() = default;

    virtual void update(Entity* notes, const Input& input, float songTime, 
                        float deltaTime) = 0;
    virtual void render(Entity* notes) = 0;
    virtual void drawUI() = 0;
    virtual void shutdown() = 0;
    virtual int  getNoteCount() const = 0;
};

#endif