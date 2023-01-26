module;

import <GL/gl.h>;

export module draw;

import vec;
import common;
import shaderprogram;
import matgl;
import glapi;
import <array>;
import <cmath>;
import <memory>;
import <vector>;
import "glfunctions.h";

using std::unique_ptr;

static std::array<GLfloat, 16> transformMatrix;
static std::array<GLfloat, 16> cameraMatrix;
static Vec camPos;
static double camPerspective;

using VAOPointer = unique_ptr<GL::VertexArrayObject>;
using VBOPointer = unique_ptr<GL::VertexBufferObject>;

static VAOPointer cometVAO;
static VAOPointer shipVAO;
static VAOPointer smokeVAO;

static VBOPointer cometVBO;
static VBOPointer shipVBO;
static VBOPointer smokeVBO;

static std::vector<GLfloat> smokeVertexData;

static class SineClass {
private:
    static const int tableLength = 1024;
    std::array<double, tableLength> table;

public:
    SineClass() {
        for (size_t i = 0; i < table.size(); ++i) {
            table[i] = sin(static_cast<double>(i) / table.size() * pi2);
        }
    }

    inline double operator()(double a) {
        a /= pi2;
        a -= floor(a);

        return table[static_cast<size_t>(tableLength * a)];
    }

    double cos(double a) {
        return (*this)(a + pi / 2);
    }

} Sine;

static const char *standardVertexShader =
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

static const char *standardFragmentShader =
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

static const char *smokeVertexShader =
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

static const char *smokeFragmentShader =
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

struct StandardShader : public ShaderProgram {
    StandardShader()
        : ShaderProgram(standardVertexShader, standardFragmentShader) {
        shaderVecPointer = getAttribute("vPosition");
        transformMatrixPointer = getUniform("model");
        cameraMatrixPointer = getUniform("view");

        if (!getProgram()) {
            throw "Could not create program.";
        }
    }

    GLint shaderVecPointer;
    GLint transformMatrixPointer;
    GLint cameraMatrixPointer;
};

class SmokeShader : public ShaderProgram {
public:
    SmokeShader() : ShaderProgram(smokeVertexShader, smokeFragmentShader) {
        shaderVecPointer = getAttribute("vPosition");
        //		transformMatrixPointer = getUniform("mvp_matrix");
        cameraMatrixPointer = getUniform("view");

        if (!getProgram()) {
            throw "Could not create program.";
        }
    }
    GLint shaderVecPointer;
    GLint cameraMatrixPointer;
};

static std::unique_ptr<StandardShader> drawShader;
static std::unique_ptr<SmokeShader> smokeShader;

inline void identityMatrix(GLfloat *matrix) {
    for (int i = 0; i < 16; ++i) {
        matrix[i] = 0;
    }
    for (int i = 0; i < 4; ++i) {
        matrix[i + i * 4] = 1;
    }
}

void modelTransform(Vec p, double a, double scale) {
    identityMatrix(transformMatrix.data());
    auto s = Sine(a);
    auto c = Sine.cos(a);

    transformMatrix[0] = static_cast<float>(c * scale);
    transformMatrix[1] = static_cast<float>(s * scale);
    transformMatrix[4] = static_cast<float>(-s * scale);
    transformMatrix[5] = static_cast<float>(c * scale);

    transformMatrix[12] = static_cast<float>(p.x);
    transformMatrix[13] = static_cast<float>(p.y);
    transformMatrix[14] = static_cast<float>(p.z);

    glCall(gl.glUniformMatrix4fv(drawShader->transformMatrixPointer,
                                 1,
                                 GL_FALSE,
                                 transformMatrix.data()));
}

void resetTransform() {
    identityMatrix(transformMatrix.data());
    glCall(gl.glUniformMatrix4fv(drawShader->transformMatrixPointer,
                                 1,
                                 GL_FALSE,
                                 transformMatrix.data()));
}
export void setCam(Vec p, double a) {
    identityMatrix(cameraMatrix.data());

    const double size = .05;
    double sx, sy;

    cameraMatrix[0] =
        static_cast<float>((sy = cos(-a)) * size / camPerspective);
    cameraMatrix[1] = static_cast<float>((sx = sin(-a)) * size);
    cameraMatrix[4] = static_cast<float>(-sx * size / camPerspective);
    cameraMatrix[5] = static_cast<float>(sy * size);

    cameraMatrix[12] =
        static_cast<float>((-p.x * sy + p.y * sx) * size / camPerspective);
    cameraMatrix[13] = static_cast<float>((-p.y * sy - p.x * sx) * size);
    cameraMatrix[14] = 1; // z - translation

    camPos = p;
    //	cameraMatrix[14] = -p.z;
}
export void camTransform() {
    drawShader->use();
    gl.glUniformMatrix4fv(
        drawShader->cameraMatrixPointer, 1, GL_FALSE, cameraMatrix.data());
    smokeShader->use();
    gl.glUniformMatrix4fv(
        smokeShader->cameraMatrixPointer, 1, GL_FALSE, cameraMatrix.data());
}

export void drawShip(Vec p, double a) {
    glCall(modelTransform(p, a, 1));
    drawShader->use();
    shipVAO->bind();

    glCall(gl.glDrawArrays(GL_TRIANGLES, 0, 3));
}

export void drawComet(Vec p, double a, double r) {
    auto dx = p.x - camPos.x;
    auto dy = p.y - camPos.y;

    if (dx * dx + dy * dy > 1500) {
        return;
    }

    cometVAO->bind();
    drawShader->use();

    modelTransform(p, a / 180., r);

    glCall(gl.glDrawArrays(GL_TRIANGLE_FAN, 0, 4));

    resetTransform();
}

export void drawStar(Vec p) {
    drawComet(p, 0, .01);
}

export void drawArea(Vec p, double a, double r) {
    cometVAO->bind();
    drawShader->use();
    modelTransform(p, a / 180., r);

    glCall(gl.glDrawArrays(GL_LINE_LOOP, 0, 4));

    resetTransform();
}

export void drawProjectile(Vec p, double a, double scale) {
    drawComet(p, a + 3.1415 / 4., scale);
}

export void drawExplosion(Vec pos, double size) {
    drawComet(pos, 0., size);
}

void pushSmoke(Vec &p1, Vec &p2, double alpha1, double alpha2) {
    auto &v = smokeVertexData;
    v.push_back(static_cast<float>(p1.x));
    v.push_back(static_cast<float>(p1.y));
    v.push_back(static_cast<float>(alpha1));

    v.push_back(static_cast<float>(p2.x));
    v.push_back(static_cast<float>(p2.y));
    v.push_back(static_cast<float>(alpha2));
}

export void drawSmoke(Vec p1, Vec p2, double alpha1, double alpha2) {
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

export void flushDraw() {
    if (smokeVertexData.empty()) {
        return;
    }

    smokeVAO->bind();
    smokeShader->use();

    smokeVBO->setData(&smokeVertexData.front(), smokeVertexData.size());
    glCall(gl.glDrawArrays(
        GL_LINES, 0, static_cast<int>(smokeVertexData.size()) / 3));

    smokeVertexData.clear();
}

export bool initDrawModule(double perspective) {
    // clang-format off
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

    // clang-format on
    const GLfloat gCometColors[] = {
        .8f,
        .8f,
        1.f,
        .8f,
    };

    drawShader.reset(new StandardShader);
    drawShader->use();

    cometVAO = std::make_unique<GL::VertexArrayObject>();
    cometVBO =
        std::make_unique<GL::VertexBufferObject>(gCometVertices, 4 * 2, 0, 2);
    int colorIndex = drawShader->getUniform("color");
    gl.glUniform4fv(colorIndex, 1, gCometColors);

    shipVAO = std::make_unique<GL::VertexArrayObject>();
    shipVBO =
        std::make_unique<GL::VertexBufferObject>(gShipVertices, 3 * 2, 0, 2);

    smokeShader.reset(new SmokeShader);
    smokeShader->use();
    smokeVAO = std::make_unique<GL::VertexArrayObject>();
    smokeVBO =
        std::make_unique<GL::VertexBufferObject>(); // Data is filled later
    smokeVBO->attribPointer(0, 3, GL_FLOAT);

    smokeVertexData.reserve(100000);
    camPerspective = perspective;

    gl.glEnable(GL_BLEND);
    gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return false;
}
