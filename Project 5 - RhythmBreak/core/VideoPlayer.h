#include "../cs3113/cs3113.h"
#include "pl_mpeg.h"

class VideoPlayer
{
private:
    plm_t*    mPlm     = nullptr;
    Texture2D mTexture = {};
    Image     mImage   = {};

public:
    void initialise(const char* fpath);
    void update(float songTime);
    void render();
    void shutdown();
};