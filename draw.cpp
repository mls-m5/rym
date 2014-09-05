#include "draw.h"
#include "glfunctions.h"
#include <math.h>
#include <vector>

#include "shaderprogram.h"

static GLuint shaderVecPointer;
GLint shaderColorPointer;
GLuint transformMatrixPointer;
GLuint cameraMatrixPointer;
static GLfloat transformMatrix[16];
static GLfloat cameraMatrix[16];
static double camPerspective;
ShaderProgram *shaderProgram;

static std::vector<GLfloat> smokeVertexData;
static std::vector<GLfloat> smokeColorData;

struct vertexDataStruct{
	GLfloat x, y;
};

struct colorDataStruct{
	GLfloat r, g, b, a;
};

static std::vector<vertexDataStruct> cometVertexData;
static std::vector<colorDataStruct> cometColorData;

static GLfloat glColors[] = {1, 1, 1, 1, 1, 1, 1, 1, .1, 1, 1, 1,1 ,1 ,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};


static const char gVertexShader[] =
    "attribute vec4 vPosition;\n"
    "attribute vec4 vColor;\n"
//    "uniform	 mat4	 pers_matrix;    //Perspective matrix \n"
    "uniform	 mat4	 mvp_matrix;	 // model-view-projection matrix\n"
    "uniform	 mat4	 proj_matrix;	 // camera matrix\n"
    "varying vec4 fColor;"
    "void main() {\n"
    "  gl_Position = proj_matrix * mvp_matrix * vPosition;\n"
    "  fColor = vColor;\n"
    "}\n";

static const char gFragmentShader[] =
    "precision mediump float;\n"
    "varying vec4 fColor;\n"
    "void main() {\n"
    "  gl_FragColor = fColor; \n"
    "}\n";


inline void identityMatrix(GLfloat *matrix){
	for (int i = 0; i < 16; ++i){
		matrix[i] = 0;
	}
	for (int i = 0; i < 4; ++i){
		matrix[i + i*4] = 1;
	}
}

void modelTransform(vec p, double a, double scale){
	identityMatrix(transformMatrix);

	transformMatrix[0] = cos(a) * scale;
	transformMatrix[1] = sin(a) * scale;
	transformMatrix[4] = -sin(a) * scale;
	transformMatrix[5] = cos(a) * scale;

	transformMatrix[12] = p.x;
	transformMatrix[13] = p.y;
	transformMatrix[14] = p.z;

    glUniformMatrix4fv(transformMatrixPointer, 1, GL_FALSE, transformMatrix);
}

void resetTransform(){
	identityMatrix(transformMatrix);
    glUniformMatrix4fv(transformMatrixPointer, 1, GL_FALSE, transformMatrix);
}
void setCam(vec p, double a){
	identityMatrix(cameraMatrix);

	const double size = .05;
	double sx, sy;

	cameraMatrix[0] = (sy = cos(-a)) * size / camPerspective;
	cameraMatrix[1] = (sx = sin(-a)) * size;
	cameraMatrix[4] = -sx * size / camPerspective;
	cameraMatrix[5] = sy * size;

	cameraMatrix[12] =  (-p.x * sy + p.y * sx) * size / camPerspective;
	cameraMatrix[13] =  (-p.y * sy - p.x * sx) * size;
//	cameraMatrix[14] = -p.z;
}

void camTransform(){
	glUniformMatrix4fv(cameraMatrixPointer, 1, GL_FALSE, cameraMatrix);
}

static const GLfloat gShipVertices[] = { 0.f, 1.f, .5f, -1.f, -.5f, -1.f };
void drawShip(vec p, double a){
	modelTransform(p, a, 1);

    glVertexAttribPointer(shaderVecPointer, 2, GL_FLOAT, GL_FALSE, 0, gShipVertices);
    glEnableVertexAttribArray(shaderVecPointer);

    glVertexAttribPointer(shaderColorPointer, 4, GL_FLOAT, GL_FALSE, 0, glColors);
    glEnableVertexAttribArray(shaderColorPointer);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    resetTransform();


    checkGlError("ship");

}
void drawStar(vec p){
	drawComet(p, 0, .01);
}

//Square
static const GLfloat gCometVertices[] = { -1.f, -1.f, 1.f, -1.f, 1.f, 1.f, -1.f, 1.f };
static const GLfloat gCometColors[] = {
		.8, .8, 1., .8,
		.8, .8, 1., .8,
		.8, .8, 1., .8,
		.8, .8, 1., .8,
};
void drawComet(vec p, double a, double r){
	modelTransform(p, a / 180., r);
    glVertexAttribPointer(shaderVecPointer, 2, GL_FLOAT, GL_FALSE, 0, gCometVertices);
    glEnableVertexAttribArray(shaderVecPointer);

    glVertexAttribPointer(shaderColorPointer, 4, GL_FLOAT, GL_FALSE, 0, gCometColors);
    glEnableVertexAttribArray(shaderColorPointer);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    resetTransform();
}

void drawProjectile(vec p, double a, double scale){
	drawComet(p, a + 3.1415 / 4., scale);
}

void drawExplosion(vec pos, double size){
	drawComet(pos, 0., size);
}

void pushSmoke(vec &p1, vec &p2, double alpha){
	smokeVertexData.push_back(p1.x);
	smokeVertexData.push_back(p1.y);
	smokeVertexData.push_back(p2.x);
	smokeVertexData.push_back(p2.y);
	smokeColorData.push_back(1);
	smokeColorData.push_back(1);
	smokeColorData.push_back(1);
	smokeColorData.push_back(alpha);

	smokeColorData.push_back(1);
	smokeColorData.push_back(1);
	smokeColorData.push_back(1);
	smokeColorData.push_back(alpha);
}

void drawSmoke(vec p1, vec p2, double alpha){
	pushSmoke(p1, p2, alpha);
}

bool initDrawModule(double perspective) {
    shaderProgram = new ShaderProgram(gVertexShader, gFragmentShader);

    if (!shaderProgram->getProgram()) {
        LOGE("Could not create program.");
        return false;
    }

    glUseProgram(shaderProgram->getProgram());
    checkGlError("glUseProgram");

	shaderVecPointer = shaderProgram->getAttribute("vPosition");
	shaderColorPointer = shaderProgram->getAttribute("vColor");
	transformMatrixPointer = shaderProgram->getUniform("mvp_matrix");
	cameraMatrixPointer = shaderProgram->getUniform("proj_matrix");

	smokeVertexData.reserve(100000);
	smokeColorData.reserve(200000);
	camPerspective = perspective;
	return false;
}

void flushDraw() {
	if (smokeVertexData.empty()){
		return;
	}
	glVertexAttribPointer(shaderVecPointer, 2, GL_FLOAT, GL_FALSE, 0, &smokeVertexData[0]);
	glEnableVertexAttribArray(shaderVecPointer);

	glVertexAttribPointer(shaderColorPointer, 4, GL_FLOAT, GL_FALSE, 0, &smokeColorData[0]);
	glEnableVertexAttribArray(shaderColorPointer);
	glDrawArrays(GL_LINES, 0, (int)smokeVertexData.size() / 2);
//	if (smokeVertexData.size() / 2 > smokeCacheReservedSize){
//		smokeVertexData.reserve(smokeCacheReservedSize * 2);
//		smokeColorData.reserve((ssmokeCacheReservedSize * 4);
//	}
//	LOGI("stortlek%d, kapacitet %d", (int) smokeVertexData.size(), (int)smokeVertexData.capacity());
	smokeVertexData.clear();
	smokeColorData.clear();
}
