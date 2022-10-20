/*
 * shaderProgram.cpp
 *
 *  Created on: 16 mar 2014
 *      Author: mattias
 */

module;

export module shaderprogram;

int loadShader(int shaderType, const char *pSource);

unsigned createProgram(const char *pVertexSource, const char *pFragmentSource);

export class ShaderProgram {
public:
    ShaderProgram(char const *vertexCode, char const *fragmentCode) {
        initProgram(vertexCode, fragmentCode);
    }

    void initProgram(char const *vertexCode, char const *fragmentCode) {
        gProgram = createProgram(vertexCode, fragmentCode);
    }

    unsigned getProgram() {
        return gProgram;
    }

    int getUniform(char const *name);

    int getAttribute(char const *name);
    void use();
    static void unuse();

    virtual ~ShaderProgram();

private:
    unsigned gProgram;
};


