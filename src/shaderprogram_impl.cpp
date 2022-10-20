module;

#include <GL/gl.h>
// #include "glfunctions.h"
#include "glfunctions.h";
#include <GL/glext.h>;
#include <vector>;

module shaderprogram;

import matgl;

GLuint createProgram(const char *pVertexSource, const char *pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                std::vector<char> buf(static_cast<size_t>(bufLength));
                glGetProgramInfoLog(program, bufLength, nullptr, &buf.front());
                LOGE("Could not link program:\n%s\n", &buf.front());
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

GLuint loadShader(GLenum shaderType, const char *pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, nullptr);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                std::vector<char> buf(static_cast<size_t>(infoLen));
                glGetShaderInfoLog(shader, infoLen, nullptr, buf.data());
                LOGE("Could not compile shader %d:\n%s\n",
                     shaderType,
                     buf.data());
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

void ShaderProgram::use() {
    glCall(glUseProgram(getProgram()));
}

void ShaderProgram::unuse() {
    glUseProgram(0);
}

ShaderProgram::~ShaderProgram() {
    if (gProgram) {
        glDeleteProgram(gProgram);
    }
}

GLint ShaderProgram::getUniform(const char *name) {
    GLint ret;
    ret = glGetUniformLocation(gProgram, name);

    checkGlError(name);
    return ret;
}

int ShaderProgram::getAttribute(const char *name) {
    GLint ret;
    ret = glGetAttribLocation(gProgram, name);

    checkGlError(name);

    return ret;
}
