// Written by Philip Rideout in May of 2010.  Covered by the MIT License.
#include <glad.h>
#include <GLFWViewer.h>
#include <vectormath_aos.h>
#include <glsw.h>
#include <openctm.h>
#include <iostream>
#include "definitions.h"

using namespace Vectormath::Aos;

enum { SlotPosition, SlotNormal };

struct ShaderUniforms {
    GLuint Projection;
    GLuint Modelview;
    GLuint NormalMatrix;
};

struct RenderContext {
    GLuint EffectHandle;
    ShaderUniforms EffectUniforms;
    Matrix4 Projection;
    Matrix4 Modelview;
    Matrix3 NormalMatrix;
    float PackedNormalMatrix[9];
    float Theta;
    CTMuint IndexCount;
};

class MyViewer: public borg::GLFWViewer {
private:
    RenderContext GlobalRenderContext;

    GLuint BuildShader(const char* source, GLenum shaderType) {
        GLint compileSuccess;
        GLchar messages[256];
        GLuint shaderHandle = glCreateShader(shaderType);
        glShaderSource(shaderHandle, 1, &source, 0);
        glCompileShader(shaderHandle);
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        // PezCheckCondition(compileSuccess, messages);
        return shaderHandle;
    }

    GLuint BuildProgram(const char* vsKey, const char* fsKey) {
        std::cout << "Compiling " << vsKey << "..." << std::endl;;
        const char* vsString = glswGetShader(vsKey);
        // PezCheckCondition(vsString, glswGetError());
        GLuint vsHandle = BuildShader(vsString, GL_VERTEX_SHADER);

        std::cout << "Compiling " << fsKey << "..." << std::endl;
        const char* fsString = glswGetShader(fsKey);
        // PezCheckCondition(fsString, glswGetError());
        GLuint fsHandle = BuildShader(fsString, GL_FRAGMENT_SHADER);

        std::cout << "Linking..." << std::endl;
        GLint linkSuccess;
        GLchar messages[256];
        GLuint programHandle = glCreateProgram();
        glAttachShader(programHandle, vsHandle);
        glAttachShader(programHandle, fsHandle);
        glBindAttribLocation(programHandle, SlotPosition, "Position");
        glBindAttribLocation(programHandle, SlotNormal, "Normal");
        glLinkProgram(programHandle);
        glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
        glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
        // PezCheckCondition(linkSuccess, messages);
        return programHandle;
    }

    void LoadMesh() {
        RenderContext& rc = GlobalRenderContext;

        // Open the CTM file:
        CTMcontext ctmContext = ctmNewContext(CTM_IMPORT);
        ctmLoad(ctmContext, "../demo/HeadlessGiant.ctm");
        // PezCheckCondition(ctmGetError(ctmContext) == CTM_NONE, "OpenCTM Issue");
        CTMuint vertexCount = ctmGetInteger(ctmContext, CTM_VERTEX_COUNT);
        rc.IndexCount = 3 * ctmGetInteger(ctmContext, CTM_TRIANGLE_COUNT);

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Create the VBO for positions:
        const CTMfloat* positions = ctmGetFloatArray(ctmContext, CTM_VERTICES);
        if (positions) {
            GLuint handle;
            GLsizeiptr size = vertexCount * sizeof(float) * 3;
            glGenBuffers(1, &handle);
            glBindBuffer(GL_ARRAY_BUFFER, handle);
            glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);
            glEnableVertexAttribArray(SlotPosition);
            glVertexAttribPointer(SlotPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        }

        // Create the VBO for normals:
        const CTMfloat* normals = ctmGetFloatArray(ctmContext, CTM_NORMALS);
        if (normals) {
            GLuint handle;
            GLsizeiptr size = vertexCount * sizeof(float) * 3;
            glGenBuffers(1, &handle);
            glBindBuffer(GL_ARRAY_BUFFER, handle);
            glBufferData(GL_ARRAY_BUFFER, size, normals, GL_STATIC_DRAW);
            glEnableVertexAttribArray(SlotNormal);
            glVertexAttribPointer(SlotNormal, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        }

        // Create the VBO for indices:
        const CTMuint* indices = ctmGetIntegerArray(ctmContext, CTM_INDICES);
        if (indices) {
            GLuint handle;
            GLsizeiptr size = rc.IndexCount * sizeof(CTMuint);
            glGenBuffers(1, &handle);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
        }

        ctmFreeContext(ctmContext);
    }

    void LoadEffect() {
        RenderContext& rc = GlobalRenderContext;

        glswInit();
        glswSetPath("../demo/", ".glsl");
        glswAddDirectiveToken("GL3", "#version 150");

        const char* vsKey = "PixelLighting.Vertex.GL3";
        const char* fsKey = "PixelLighting.Fragment.GL3";

        rc.EffectHandle = BuildProgram(vsKey, fsKey);
        rc.EffectUniforms.Projection = glGetUniformLocation(rc.EffectHandle, "Projection");
        rc.EffectUniforms.Modelview = glGetUniformLocation(rc.EffectHandle, "Modelview");
        rc.EffectUniforms.NormalMatrix = glGetUniformLocation(rc.EffectHandle, "NormalMatrix");
        rc.Theta = 0;

        glUseProgram(rc.EffectHandle);

        GLuint LightPosition = glGetUniformLocation(rc.EffectHandle, "LightPosition");
        GLuint AmbientMaterial = glGetUniformLocation(rc.EffectHandle, "AmbientMaterial");
        GLuint DiffuseMaterial = glGetUniformLocation(rc.EffectHandle, "DiffuseMaterial");
        GLuint SpecularMaterial = glGetUniformLocation(rc.EffectHandle, "SpecularMaterial");
        GLuint Shininess = glGetUniformLocation(rc.EffectHandle, "Shininess");

        glUniform3f(DiffuseMaterial, 0.75, 0.75, 0.5);
        glUniform3f(AmbientMaterial, 0.04f, 0.04f, 0.04f);
        glUniform3f(SpecularMaterial, 0.5, 0.5, 0.5);
        glUniform1f(Shininess, 50);
        glUniform3f(LightPosition, 0.25, 0.25, 1);
    }

public:
    void setup() {
        LoadMesh();
        LoadEffect();
        glEnable(GL_DEPTH_TEST);
    }

    void render() {
        RenderContext& rc = GlobalRenderContext;
        glClearColor(0, 0.25f, 0.5f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniformMatrix4fv(rc.EffectUniforms.Projection, 1, 0, &rc.Projection[0][0]);
        glUniformMatrix4fv(rc.EffectUniforms.Modelview, 1, 0, &rc.Modelview[0][0]);
        glUniformMatrix3fv(rc.EffectUniforms.NormalMatrix, 1, 0, &rc.PackedNormalMatrix[0]);
        glDrawElements(GL_TRIANGLES, rc.IndexCount, GL_UNSIGNED_INT, 0);
    }

    void update() {
        RenderContext& rc = GlobalRenderContext;

        rc.Theta += m_delta * 50.f;

        Matrix4 rotation = Matrix4::rotationY(rc.Theta * borg::PI / 180.0f);
        Matrix4 translation = Matrix4::translation(Vector3(0, 0, -50));
        rc.Modelview = translation * rotation;
        rc.NormalMatrix = rc.Modelview.getUpper3x3();

        for (int i = 0; i < 9; ++i)
            rc.PackedNormalMatrix[i] = rc.NormalMatrix[i / 3][i % 3];



        const float x = 0.6f;
        const float y = x * m_frameBufferHeight / m_frameBufferWidth;
        const float left = -x, right = x;
        const float bottom = -y, top = y;
        const float zNear = 4, zFar = 100;
        rc.Projection = Matrix4::frustum(left, right, bottom, top, zNear, zFar);
    }
};

borg::GLFWViewer* g_viewer;

int main(void)
{
    g_viewer = new MyViewer();
    bool initOK = g_viewer->init("CTM Viewer", 640, 480);
    if (!initOK) {
        std::cout << "Error in init" << std::endl;
        exit(EXIT_FAILURE);
    }

    g_viewer->mainLoop();
}

