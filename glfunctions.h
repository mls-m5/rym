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

#ifdef __APPLE__
#include <OpenGLES/ES3/glext.h>
#else

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
//From win/GL -folder


#include <GL/glew.h>

#else
#include <GL/gl.h>
#include <GL/glext.h>

#endif

#endif //apple

#include <cstdio>


#define  LOGI(...)  printf(__VA_ARGS__)
#define  LOGE(...)  printf(__VA_ARGS__)

#endif

#include <stdexcept>
#include <string>

static void printGLString(const char *name, GLenum s) {
    auto *v = glGetString(s);
    LOGE("GL %s = %s\n", name, v);
}

inline int checkGlError(const char* op, bool throwError = true) {
#ifndef NO_GRAPHICS
	bool ret = false;
    for (auto error = glGetError(); error; error
            = glGetError()) {
        const char * c = nullptr;
    	switch (error) {
        case 0x0500:
    		c = "GL_INVALID_ENUM";
    		break;
    	case 0x0501:
    		c = "GL_INVALID_VALUE";
    		break;
    	case 0x0502:
    		c = "GL_INVALID_OPERATION";
    		break;
    	case 0x0503:
    		c = "GL_STACK_OVERFLOW";
    		break;
    	case 0x0504:
    		c = "GL_STACK_UNDERFLOW";
    		break;
    	case 0x0505:
    		c = "GL_OUT_OF_MEMORY";
    		break;
    	case 0x0506:
    		c = "GL_INVALID_FRAMEBUFFER_OPERATION";
    		break;
    	case 0x0507:
    		c = "GL_CONTEXT_LOST";
    		break;
    	case 0x8031:
    		c = "GL_TABLE_TOO_LARGE1";
    		break;
    	}
        printf("after %s()\n glError (0x%x) %s \n\n", op, error, c);
        printGLString(op, error);
        if (throwError) {
            throw std::runtime_error("Opengl error: " + std::string(c));
        }
    }
    return ret;
#endif
}

#define glCall(call) call; checkGlError(#call)
