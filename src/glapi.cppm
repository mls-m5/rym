module;

export module glapi;

import <GL/gl.h>;
import <SDL2/SDL.h>;
import <stdexcept>;
import <iostream>;

#include "glfunctions.h"

export struct Gl {

    void (*glUniformMatrix4fv)(GLint location,
                               GLsizei count,
                               GLboolean transpose,
                               const GLfloat *value) = 0;

    void (*glUniform4fv)(GLint location, GLsizei count, const GLfloat *value);

    void (*glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
    void (*glGetShaderInfoLog)(GLuint shader,
                               GLsizei bufSize,
                               GLsizei *length,
                               GLchar *infoLog);

    GLuint (*glCreateProgram)(void);
    void (*glAttachShader)(GLuint program, GLuint shader);
    void (*glLinkProgram)(GLuint program);
    void (*glGetProgramiv)(GLuint program, GLenum pname, GLint *params);
    void (*glUseProgram)(GLuint program);
    void (*glDeleteShader)(GLuint shader);
    GLint (*glGetUniformLocation)(GLuint program, const GLchar *name);
    void (*glDeleteProgram)(GLuint program);
    void (*glCompileShader)(GLuint shader);
    GLuint (*glCreateShader)(GLenum type);
    GLint (*glGetAttribLocation)(GLuint program, const GLchar *name);
    void (*glShaderSource)(GLuint shader,
                           GLsizei count,
                           const GLchar *const *string,
                           const GLint *length);
    void (*glGetProgramInfoLog)(GLuint program,
                                GLsizei bufSize,
                                GLsizei *length,
                                GLchar *infoLog);

    void (*glGenVertexArrays)(GLsizei n, GLuint *arrays);

    void (*glDeleteVertexArrays)(GLsizei n, const GLuint *arrays);

    void (*glBindVertexArray)(GLuint array);
    void (*glGenBuffers)(GLsizei n, GLuint *buffers);
    void (*glDeleteBuffers)(GLsizei n, const GLuint *buffers);
    void (*glBindBuffer)(GLenum target, GLuint buffer);
    void (*glVertexAttribPointer)(GLuint index,
                                  GLint size,
                                  GLenum type,
                                  GLboolean normalized,
                                  GLsizei stride,
                                  const void *pointer);

    void (*glEnableVertexAttribArray)(GLuint index);

    void (*glBufferData)(GLenum target,
                         GLsizeiptr size,
                         const void *data,
                         GLenum usage);

    // ...
    void (*glEnable)(GLenum cap);
    void (*glClear)(GLbitfield mask);
    void (*glDrawArrays)(GLenum mode, GLint first, GLsizei count);
    const GLubyte *(*glGetString)(GLenum name);
    GLenum (*glGetError)(void);
    void (*glBlendFunc)(GLenum sfactor, GLenum dfactor);

    Gl() {
        SDL_Init(SDL_INIT_VIDEO);
        if (SDL_GL_LoadLibrary(nullptr)) {
            std::cerr << "could not load open gl library\n";
            std::cerr << SDL_GetError() << "\n";
            std::exit(1);
        }

#define load(name)                                                             \
    name = reinterpret_cast<decltype(name)>(SDL_GL_GetProcAddress(#name))

        load(glUniformMatrix4fv);
        load(glUniform4fv);
        load(glGetShaderiv);
        load(glGetShaderInfoLog);
        load(glCreateProgram);
        load(glAttachShader);
        load(glLinkProgram);
        load(glGetProgramiv);
        load(glUseProgram);
        load(glDeleteShader);
        load(glGetUniformLocation);
        load(glDeleteProgram);
        load(glCompileShader);
        load(glCreateShader);
        load(glGetAttribLocation);
        load(glShaderSource);
        load(glGetProgramInfoLog);
        load(glGenVertexArrays);
        load(glDeleteVertexArrays);
        load(glBindVertexArray);
        load(glGenBuffers);
        load(glDeleteBuffers);
        load(glBindBuffer);
        load(glVertexAttribPointer);
        load(glEnableVertexAttribArray);
        load(glBufferData);
        load(glEnable);
        load(glClear);
        load(glDrawArrays);
        load(glGetString);
        load(glGetError);
        load(glBlendFunc);
    }
};

#undef load // For when haxxor-building without modules

export Gl gl;

export template <typename... Args>
void LOGI(Args... args) {
}

export template <typename... Args>
void LOGE(Args... args) {
}

static void printGLString(const char *name, GLenum s) {
    auto *v = gl.glGetString(s);
    //    LOGE("GL %s = %s\n", name, v);
    LOGE("GL ", name, " = ", v);
}

export int checkGlError(const char *op, bool throwError = true) {
#ifndef NO_GRAPHICS
    bool ret = false;
    for (auto error = gl.glGetError(); error; error = gl.glGetError()) {
        const char *c = nullptr;
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
