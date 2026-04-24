#include "Scene.h"
#include "../modes/CircleMode.h"
#include "../modes/ScanlineMode.h"
#include "../core/VideoPlayer.h"
#include "../core/ShaderProgram.h"

class BeatmapScene : public Scene
{
public:
    BeatmapScene();
    BeatmapScene(Vector2 origin, const char *bgHexCode);
    ~BeatmapScene();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

private:
    Clock       mClock;
    Beatmap     mBeatmap;
    Scoreboard  mScoreboard;
    VideoPlayer mVideoPlayer;

    Sound mFallingHitSound = {};
    Sound mCircleHitSound  = {};

    Texture2D mCursorTexture = {};

    Entity* mNotes = nullptr;  // note pool for all beatmap notes

    int  mCurrSection = 0;
    int  mNumNotes    = 0;
    bool mIsPlaying   = false;

    int mLastSoundHitCount = 0; 
    int mNumApproachNotes  = 0;

    float mFadeTimer = 0.0f;
    bool  mFading    = false;

    FallingMode  mFallingMode;
    CircleMode   mCircleMode;
    ScanlineMode mScanlineMode;
    GameMode*    mActiveMode = nullptr;

    ShaderProgram mShader;
    bool mShaderActive = false;

    // Switch the active section and mode
    void switchSection(int sectionIndex);
};