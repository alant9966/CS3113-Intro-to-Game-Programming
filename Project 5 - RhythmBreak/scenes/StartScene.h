#include "Scene.h"

#ifndef STARTSCENE_H
#define STARTSCENE_H

class StartScene : public Scene
{
public:
    StartScene();
    StartScene(Vector2 origin, const char *bgHexCode);
    ~StartScene();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

private:
    Texture2D mBgTexture  = {};
    Music     mBgMusic    = {};
};

#endif