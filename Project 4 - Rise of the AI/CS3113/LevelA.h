#include "Scene.h"

constexpr int LEVEL_WIDTH_A  = 24,
              LEVEL_HEIGHT_A = 12;

class LevelA : public Scene {
private:
    unsigned int mLevelData[LEVEL_WIDTH_A * LEVEL_HEIGHT_A] = {
        0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0, 0,  0, 0, 0,
        0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0, 0,  0, 0, 0,
        0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0, 0,  0, 0, 0,
        0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0, 0,  0, 0, 0,
        0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0, 0,  0, 0, 0,
        0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0, 0,  0, 0, 0,
        0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0, 0,  0, 0, 0,
        0, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 11, 12, 12, 12, 12, 13, 0, 0,  0, 0, 0,
        0, 0,  0,  0,  0,  0, 0, 11, 12, 12, 12, 13, 0, 28, 29, 29, 29, 29, 30, 0, 0, 32, 0, 0,
        0, 0, 11, 12, 12, 13, 0, 28, 29, 29, 29, 30, 0, 28, 29, 29, 29, 29, 30, 0, 0, 14, 0, 0,
        0, 0, 28, 29, 29, 30, 0, 28, 29, 29, 29, 30, 0, 28, 29, 29, 29, 29, 30, 0, 0, 31, 0, 0,
        0, 0, 28, 29, 29, 30, 0, 28, 29, 29, 29, 30, 0, 28, 29, 29, 29, 29, 30, 0, 0, 31, 0, 0,
    };

public:
    static constexpr float TILE_DIMENSION          = 75.0f,
                           ACCELERATION_OF_GRAVITY = 981.0f,
                           END_GAME_THRESHOLD      = 800.0f;

    LevelA();
    LevelA(Vector2 origin, const char *bgHexCode);
    ~LevelA();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};