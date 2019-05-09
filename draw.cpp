#include "draw.h"
#include "glfunctions.h"
#include <math.h>
#include <vector>
#include <memory>

#include "shaderprogram.h"
#include "matgl.h"

static GLfloat transformMatrix[16];
static GLfloat cameraMatrix[16];
static Vec camPos;
static double camPerspective;

#define up std::unique_ptr
typedef up<GL::VertexArrayObject> VAOPointer;
typedef up<GL::VertexBufferObject> VBOPointer;

static VAOPointer cometVAO;
static VAOPointer shipVAO;
static VAOPointer smokeVAO;

static VBOPointer cometVBO;
static VBOPointer shipVBO;
static VBOPointer smokeVBO;

static std::vector<GLfloat> smokeVertexData;

class SineClass{
public:
	static const int tableLength = 1024;
	SineClass(){
		table = new double [tableLength + 1];
		for (int i = 0; i < tableLength; ++i){
			table[i] = sin((double)i / tableLength * pi2);
		}
	}

	~SineClass(){
		delete table;
	}

	inline double operator()(double a){
		a /= pi2;
		a -= floor(a);

		return table[(int)(tableLength * a)];
	}

	double cos(double a){
		return (*this)(a + pi / 2);
	}

	double *table;

} Sine;


static const char standardVertexShader[] =
R"_(
#version 330 core
layout (location = 0) in vec4 vPosition;
uniform	 mat4 model;	 // model-view-projection matrix
uniform	 mat4 view;	 // camera matrix
void main() {
	gl_Position = view * model * vPosition;
	float perspective = (gl_Position.z + gl_Position.y / 5.);
	gl_Position.x /= perspective;
	gl_Position.y /= perspective;
}
)_";

static const char standardFragmentShader[] =
R"_(
#version 330 core
uniform vec4 color;
void main() {
  if (gl_FragCoord.z < 0.1) {
    discard;
  }
  gl_FragColor = color;
}

)_";


static const char smokeVertexShader[] =
R"_(


#version 330 core
layout (location = 0) in vec3 vPosition;
out float fAlpha;
uniform	 mat4 view;	 // camera matrix
void main() {
	fAlpha = vPosition.z;
	gl_Position = view *vec4(vPosition.xy, 0, 1);
	float perspective = (gl_Position.z + gl_Position.y / 5.);
	gl_Position.x /= perspective;
	gl_Position.y /= perspective;
}

)_";

static const char smokeFragmentShader[] =
R"_(
#version 330 core

in float fAlpha;

void main() {
	if (fAlpha < .1) {
		discard;
	}
	gl_FragColor = vec4(1, 1, 1, fAlpha);
}

)_";


class StandardShader: public ShaderProgram {
public:

	StandardShader(): ShaderProgram(standardVertexShader, standardFragmentShader) {
		shaderVecPointer = getAttribute("vPosition");
		transformMatrixPointer = getUniform("model");
		cameraMatrixPointer = getUniform("view");

	    if (!getProgram()) {
	        throw "Could not create program.";
	    }
	}

	GLuint shaderVecPointer;
	GLuint transformMatrixPointer;
	GLuint cameraMatrixPointer;
};

class SmokeShader: public ShaderProgram {
public:
	SmokeShader(): ShaderProgram(smokeVertexShader, smokeFragmentShader)  {
		shaderVecPointer = getAttribute("vPosition");
//		transformMatrixPointer = getUniform("mvp_matrix");
		cameraMatrixPointer = getUniform("view");

	    if (!getProgram()) {
	        throw "Could not create program.";
	    }
	}
	GLuint shaderVecPointer;
//	GLuint transformMatrixPointer;
	GLuint cameraMatrixPointer;
};

std::unique_ptr<StandardShader> drawShader;
std::unique_ptr<SmokeShader> smokeShader;

inline void identityMatrix(GLfloat *matrix){
	for (int i = 0; i < 16; ++i){
		matrix[i] = 0;
	}
	for (int i = 0; i < 4; ++i){
		matrix[i + i*4] = 1;
	}
}

void modelTransform(Vec p, double a, double scale){
	identityMatrix(transformMatrix);
	auto s = Sine(a);
	auto c = Sine.cos(a);
//	auto s = sin(a);
//	auto c = cos(a);

	transformMatrix[0] = c * scale;
	transformMatrix[1] = s * scale;
	transformMatrix[4] = -s * scale;
	transformMatrix[5] = c * scale;

	transformMatrix[12] = p.x;
	transformMatrix[13] = p.y;
	transformMatrix[14] = p.z;

    glCall(glUniformMatrix4fv(drawShader->transformMatrixPointer, 1, GL_FALSE, transformMatrix));
}

void resetTransform(){
	identityMatrix(transformMatrix);
    glCall(glUniformMatrix4fv(drawShader->transformMatrixPointer, 1, GL_FALSE, transformMatrix));
}
void setCam(Vec p, double a){
	identityMatrix(cameraMatrix);

	const double size = .05;
	double sx, sy;

	cameraMatrix[0] = (sy = cos(-a)) * size / camPerspective;
	cameraMatrix[1] = (sx = sin(-a)) * size;
	cameraMatrix[4] = -sx * size / camPerspective;
	cameraMatrix[5] = sy * size;

	cameraMatrix[12] =  (-p.x * sy + p.y * sx) * size / camPerspective;
	cameraMatrix[13] =  (-p.y * sy - p.x * sx) * size;
	cameraMatrix[14] = 1; //z - translation

	camPos = p;
//	cameraMatrix[14] = -p.z;
}

void camTransform(){
	drawShader->use();
	glUniformMatrix4fv(drawShader->cameraMatrixPointer, 1, GL_FALSE, cameraMatrix);
	smokeShader->use();
	glUniformMatrix4fv(smokeShader->cameraMatrixPointer, 1, GL_FALSE, cameraMatrix);
}

void drawShip(Vec p, double a){
	glCall(modelTransform(p, a, 1));
	drawShader->use();
	shipVAO->bind();

    glCall(glDrawArrays(GL_TRIANGLES, 0, 3));
}
void drawStar(Vec p){
	drawComet(p, 0, .01);
}

void drawComet(Vec p, double a, double r){
	auto dx = p.x - camPos.x;
	auto dy = p.y - camPos.y;

	if (dx * dx + dy * dy > 1500){
		return;
	}

	cometVAO->bind();
	drawShader->use();

	modelTransform(p, a / 180., r);

    glCall(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    resetTransform();
}


void drawArea(Vec p, double a, double r){
	auto dx = p.x - camPos.x;
	auto dy = p.y - camPos.y;

	cometVAO->bind();
	drawShader->use();
	modelTransform(p, a / 180., r);

    glCall(glDrawArrays(GL_LINE_LOOP, 0, 4));

    resetTransform();
}

void drawProjectile(Vec p, double a, double scale){
	drawComet(p, a + 3.1415 / 4., scale);
}

void drawExplosion(Vec pos, double size){
	drawComet(pos, 0., size);
}

void pushSmoke(Vec &p1, Vec &p2, float alpha1, float alpha2){
	auto &v = smokeVertexData;
	v.push_back(p1.x);
	v.push_back(p1.y);
	v.push_back(alpha1);

	v.push_back(p2.x);
	v.push_back(p2.y);
	v.push_back(alpha2);
}

void drawSmoke(Vec p1, Vec p2, double alpha1, double alpha2){
	auto dx = p1.x - camPos.x;
	auto dy = p1.y - camPos.y;
	constexpr double maxDistance = 50;
	if (dx > maxDistance or dy > maxDistance) {
		return;
	}
	if (dx < -maxDistance or dy < -maxDistance) {
		return;
	}
	pushSmoke(p1, p2, alpha1, alpha2);
}


void flushDraw() {
	if (smokeVertexData.empty()){
		return;
	}

	smokeVAO->bind();
	smokeShader->use();
//	drawShader->use();
//	modelTransform({}, 0, 0);

	smokeVBO->setData(&smokeVertexData.front(), smokeVertexData.size());
//	smokeVBO->attribPointer(0, 3, GL_FLOAT, false);
	glCall(glDrawArrays(GL_LINES, 0, (int)smokeVertexData.size() / 3));

	smokeVertexData.clear();


}


bool initDrawModule(double perspective) {
	const GLfloat gShipVertices[] = {
			0.f, 1.f,
			.5f, -1.f,
			-.5f, -1.f
	};

	//Square
	const GLfloat gCometVertices[] = {
			-1.f, -1.f,
			1.f, -1.f,
			1.f, 1.f,
			-1.f, 1.f
	};
	const GLfloat gCometColors[] = {
			.8, .8, 1., .8,
	};

    drawShader.reset(new StandardShader);
    drawShader->use();


	cometVAO = std::make_unique<GL::VertexArrayObject>();
	cometVBO = std::make_unique<GL::VertexBufferObject>(gCometVertices, 4 * 2, 0, 2);
	int colorIndex = drawShader->getUniform("color");
	glUniform4fv(colorIndex, 1, gCometColors);

	shipVAO = std::make_unique<GL::VertexArrayObject>();
	shipVBO = std::make_unique<GL::VertexBufferObject>(gShipVertices, 3 * 2, 0, 2);

	smokeShader.reset(new SmokeShader);
	smokeShader->use();
	smokeVAO = std::make_unique<GL::VertexArrayObject>();
	smokeVBO = std::make_unique<GL::VertexBufferObject>(); //Data is filled later
	smokeVBO->attribPointer(0, 3, GL_FLOAT);


	smokeVertexData.reserve(100000);
	camPerspective = perspective;

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return false;
}
