#define PL_MPEG_IMPLEMENTATION
#include "VideoPlayer.h"

void VideoPlayer::initialise(const char* fpath)
{
    // Open the video file and create a plm_t instance
    FILE* f = fopen(fpath, "rb");
    if (!f) {
        LOG("VideoPlayer: Failed to open " << fpath);
        return;
    }
    mPlm = plm_create_with_file(f, true);

    // Check that header decoding is successful
    if (!plm_has_headers(mPlm)) {
        LOG("VideoPlayer: Failed to decode stream headers");
        plm_destroy(mPlm);
        mPlm = nullptr;
        return;
    }

    // Load the video frame texture
    mImage   = GenImageColor(plm_get_width(mPlm), plm_get_height(mPlm), BLACK);
    mTexture = LoadTextureFromImage(mImage);
}

void VideoPlayer::update(float songTime)
{
    if (!mPlm) return;

    double videoTime  = plm_get_time(mPlm);
    double targetTime = std::fmod((double)songTime, plm_get_duration(mPlm));

    // Handle song looping
    if (targetTime < videoTime - 1.0) {
        plm_rewind(mPlm);
        videoTime = 0.0;
    }

    // Decode frames until video time matches song time
    while (plm_get_time(mPlm) < targetTime) {
        plm_frame_t* frame = plm_decode_video(mPlm);
        if (!frame) break;

        plm_frame_to_rgba(frame, (uint8_t*)mImage.data, plm_get_width(mPlm) * 4);
    }

    UpdateTexture(mTexture, mImage.data);
}

void VideoPlayer::render()
{
    if (!mTexture.id) return;

    DrawTexturePro(
        mTexture,
        { 0, 0, (float)mTexture.width, (float)mTexture.height },
        { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT },
        { 0, 0 }, 0.0f, WHITE
    );
}

void VideoPlayer::shutdown()
{
    plm_destroy(mPlm);
    UnloadTexture(mTexture);
    UnloadImage(mImage);
}