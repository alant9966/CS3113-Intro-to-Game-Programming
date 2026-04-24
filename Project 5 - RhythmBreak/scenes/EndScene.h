#include "Scene.h"

#ifndef ENDSCENE_H
#define ENDSCENE_H

class EndScene : public Scene
{
public:
    EndScene();
    EndScene(Vector2 origin, const char *bgHexCode);
    ~EndScene();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

private:
    Texture2D mBgTexture = {};
    Music     mBgMusic   = {};

    bool  mPassed   = false;
    float mAccuracy = 0.0f;
    int   mScore    = 0;
    int   mMaxCombo = 0;

    // Computes a letter grade from accuracy
    const char* getGrade() const;
};

#endif