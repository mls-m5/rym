/*
 * shaderProgram.h
 *
 *  Created on: 16 mar 2014
 *      Author: mattias
 */

#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

#include "glfunctions.h"


class ShaderProgram {
public:
	ShaderProgram();
	ShaderProgram(char const*vertexCode, char const*fragmentCode);
	void initProgram(char const*vertexCode, char const*fragmentCode);
	void loadVertexShader(char *code);
	void loadFragmentShader(char *code);
	void linkProgram();

	GLuint getProgram() { return gProgram; };
	GLint getUniform( char const* name );
	GLint getAttribute( char const* name );
	void use();
	static void unuse();
	virtual ~ShaderProgram();

private:
	GLuint gProgram;
};

#endif /* SHADERPROGRAM_H_ */
