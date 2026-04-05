#include "WinScene.h"

class LoseScene : public Scene {
private:
    Texture2D mBgTexture;

public:
    LoseScene();
    LoseScene(Vector2 origin, const char *bgHexCode);
    ~LoseScene();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};