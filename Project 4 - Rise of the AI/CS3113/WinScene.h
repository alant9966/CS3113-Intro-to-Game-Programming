#include "Menu.h"

class WinScene : public Scene {
private:
    Texture2D mBgTexture;

public:
    WinScene();
    WinScene(Vector2 origin, const char *bgHexCode);
    ~WinScene();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};