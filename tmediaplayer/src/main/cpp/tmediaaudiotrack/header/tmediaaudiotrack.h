
#ifndef TMEDIAPLAYER_TMEDIAFRAMELOADER_H
#define TMEDIAPLAYER_TMEDIAFRAMELOADER_H

#include <jni.h>
#include "tmediaplayer.h"

extern "C" {
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
}

typedef struct tMediaAudioTrackContext {
    SLObjectItf engineObject = nullptr;
    SLEngineItf engineInterface = nullptr;

    SLObjectItf outputMixObject = nullptr;

    SLObjectItf playerObject = nullptr;
    SLPlayItf playerInterface = nullptr;
    SLAndroidSimpleBufferQueueItf playerBufferQueueInterface = nullptr;

    SLuint32 inputQueueSize = 10;
    SLuint32 inputSampleChannels = 2;
    SLuint32 inputSampleRate = SL_SAMPLINGRATE_48;
    SLuint32 inputSampleFormat = SL_PCMSAMPLEFORMAT_FIXED_16;

    JavaVM *jvm = nullptr;

    tMediaOptResult prepare();

    tMediaOptResult play();

    tMediaOptResult pause();

    tMediaOptResult enqueueBuffer(tMediaAudioBuffer* buffer);

    tMediaOptResult clearBuffers();

    void release();

} tMediaAudioTrackContext;

#endif