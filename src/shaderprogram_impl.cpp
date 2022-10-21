module;

#include "glfunctions.h"
#include <GL/gl.h>
// #include <GL/glext.h>;
// #include <SDL2/SDL_opengl_glext.h>

module shaderprogram;

import matgl;
import glapi;
import <vector>;
// import <iostream>;

#define glCall(x) x;

GLuint createProgram(const char *pVertexSource, const char *pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = gl.glCreateProgram();
    if (program) {
        gl.glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        gl.glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        gl.glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        gl.glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            gl.glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                std::vector<char> buf(static_cast<size_t>(bufLength));
                gl.glGetProgramInfoLog(
                    program, bufLength, nullptr, &buf.front());
                LOGE("Could not link program:\n%s\n", &buf.front());
            }
            gl.glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

GLuint loadShader(GLenum shaderType, const char *pSource) {
    GLuint shader = gl.glCreateShader(shaderType);
    if (shader) {
        gl.glShaderSource(shader, 1, &pSource, nullptr);
        gl.glCompileShader(shader);
        GLint compiled = 0;
        gl.glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            gl.glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                std::vector<char> buf(static_cast<size_t>(infoLen));
                gl.glGetShaderInfoLog(shader, infoLen, nullptr, buf.data());
                LOGE("Could not compile shader %d:\n%s\n",
                     shaderType,
                     buf.data());
                gl.glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

void ShaderProgram::use() {
    glCall(gl.glUseProgram(getProgram()));
}

void ShaderProgram::unuse() {
    gl.glUseProgram(0);
}

ShaderProgram::~ShaderProgram() {
    if (gProgram) {
        gl.glDeleteProgram(gProgram);
    }
}

GLint ShaderProgram::getUniform(const char *name) {
    GLint ret;
    ret = gl.glGetUniformLocation(gProgram, name);

    checkGlError(name);
    return ret;
}

int ShaderProgram::getAttribute(const char *name) {
    GLint ret;
    ret = gl.glGetAttribLocation(gProgram, name);

    checkGlError(name);

    return ret;
}
