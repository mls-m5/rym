#pragma once

#if defined(__ANDROID__)
#include <android/log.h>
#include <GLES2/gl2.h>

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#else
#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#if __APPLE__
#include <OpenGLES/ES3/glext.h>
#else

#include <GL/gl.h>
#include <GL/glext.h>

#endif //apple

#include <stdio.h>


#define  LOGI(...)  printf(__VA_ARGS__)
#define  LOGE(...)  printf(__VA_ARGS__)

#endif

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGE("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGE("after %s() glError (0x%x)\n", op, error);
    }
}

