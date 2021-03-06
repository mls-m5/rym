/*
 * shaderProgram.cpp
 *
 *  Created on: 16 mar 2014
 *      Author: mattias
 */

#include "glfunctions.h"
#include "matgl.h"

export module shaderprogram;

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

export class ShaderProgram {
public:
    ShaderProgram();
    ShaderProgram(char const *vertexCode, char const *fragmentCode) {
        initProgram(vertexCode, fragmentCode);
    }

    void initProgram(char const *vertexCode, char const *fragmentCode) {
        gProgram = createProgram(vertexCode, fragmentCode);
    }

    //    void loadVertexShader(char *code);
    //    void loadFragmentShader(char *code);
    //    void linkProgram();

    GLuint getProgram() {
        return gProgram;
    }
    GLint getUniform(char const *name) {
        GLint ret;
        ret = glGetUniformLocation(gProgram, name);

        checkGlError(name);
        return ret;
    }
    GLint getAttribute(char const *name) {
        GLint ret;
        ret = glGetAttribLocation(gProgram, name);

        checkGlError(name);

        return ret;
    }
    void use() {
        glCall(glUseProgram(getProgram()));
    }
    static void unuse() {
        glUseProgram(0);
    }
    virtual ~ShaderProgram() {
        if (gProgram) {
            glDeleteProgram(gProgram);
        }
    }

private:
    GLuint gProgram;
};
