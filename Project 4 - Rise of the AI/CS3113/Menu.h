#include "Scene.h"

class Menu : public Scene {
private:
    Texture2D mBgTexture;

public:
    Menu();
    Menu(Vector2 origin, const char *bgHexCode);
    ~Menu();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};