#pragma once

#if defined(__ANDROID__)
#include <GLES2/gl2.h>
#include <android/log.h>

#define LOG_TAG "libgl2jni"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#else
#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#ifdef __APPLE__
#include <OpenGLES/ES3/glext.h>
#else

#if defined(WIN32) || defined(_WIN32) ||                                       \
    defined(__WIN32) && !defined(__CYGWIN__)
// From win/GL -folder

#include <GL/glew.h>

#else
#include <GL/gl.h>
#include <GL/glext.h>

#endif

#endif // apple

#include <cstdio>

#define LOGI(...) printf(__VA_ARGS__)
#define LOGE(...) printf(__VA_ARGS__)

#endif

#include <stdexcept>
#define glCall(call)                                                           \
    call;                                                                      \
    checkGlError(#call)
