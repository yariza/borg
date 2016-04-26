#include <pez.h>
#include <gl_core_4_1.h>
#include <glsw.h>

static void BuildGeometry();
static void LoadEffect();

enum { PositionSlot, ColorSlot };

void PezHandleMouse(int x, int y, int action) { }

void PezUpdate(unsigned int elapsedMilliseconds) { }

void PezRender()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    GLenum err;
    // PezCheckCondition(!(err = glGetError()), "Error drawing Arrays: %#06x\n", err);
    // if ((error = glGetError())) {
    //     printf("GL ERROR: %d\n", error);
    // }
}

const char* PezInitialize(int width, int height)
{
    BuildGeometry();
    LoadEffect();
    return "Pez Intro";
}

static void BuildGeometry()
{
    float verts[] = {
        -0.5f, -0.5f,  1,1,0, // Yellow
        +0.0f, +0.5f,  1,0,1, // Magenta
        +0.5f, -0.5f,  0,1,1, // Cyan
    };

    GLuint vboHandle;
    GLuint vao;
    GLsizeiptr vboSize = sizeof(verts);
    GLsizei stride = 5 * sizeof(float);
    GLenum usage = GL_STATIC_DRAW;
    GLvoid* colorOffset = (GLvoid*) (sizeof(float) * 2);
    GLenum err;

    glGenBuffers(1, &vboHandle);
    PezCheckCondition(!(err = glGetError()), "Error genBuffers: %#06x\n", err);
    glGenVertexArrays(1, &vao);
    PezCheckCondition(!(err = glGetError()), "Error glGenVertexArrays: %#06x\n", err);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    glBindVertexArray(vao);
    PezCheckCondition(!(err = glGetError()), "Error bindBuffer: %#06x\n", err);
    glBufferData(GL_ARRAY_BUFFER, vboSize, verts, usage);
    PezCheckCondition(!(err = glGetError()), "Error bufferData: %#06x\n", err);
    glVertexAttribPointer(PositionSlot, 2, GL_FLOAT, GL_FALSE, stride, 0);
    PezCheckCondition(!(err = glGetError()), "Error vertexAttribPointer: %#06x\n", err);
    glVertexAttribPointer(ColorSlot, 3, GL_FLOAT, GL_FALSE, stride, colorOffset);
    PezCheckCondition(!(err = glGetError()), "Error 2vertexAttribPointer: %#06x\n", err);
    glEnableVertexAttribArray(PositionSlot);
    glEnableVertexAttribArray(ColorSlot);
    PezCheckCondition(!(err = glGetError()), "Error enableVertexAttribArray: %#06x\n", err);

    // GLuint shaderProgram;
    // GLuint vertexArrayObject;
    // GLuint vertexBuffer;

    // GLint positionUniform;
    // GLint colourAttribute;
    // GLint positionAttribute;

    // glGenVertexArrays(1, &vertexArrayObject);
    // glBindVertexArray(vertexArrayObject);

    // glGenBuffers(1, &vertexBuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    // glBufferData(GL_ARRAY_BUFFER, 3*5*sizeof(GLfloat), verts, GL_STATIC_DRAW);

    // glEnableVertexAttribArray((GLuint)positionAttribute);
    // glEnableVertexAttribArray((GLuint)colourAttribute  );
    // glVertexAttribPointer((GLuint)positionAttribute, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0);
    // glVertexAttribPointer((GLuint)colourAttribute  , 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0+2*sizeof(GLfloat));

}

static void LoadEffect()
{
    const char* vsSource, * fsSource;
    GLuint vsHandle, fsHandle;
    GLint compileSuccess, linkSuccess;
    GLchar compilerSpew[256];
    GLuint programHandle;
    GLenum err;

    glswInit();
    glswSetPath("../demo/", ".glsl");
    glswAddDirectiveToken("GL3", "#version 150");
    glswAddDirectiveToken("GL2", "#version 120");

    vsSource = glswGetShader("Simple.Vertex." PEZ_GL_VERSION_TOKEN);
    fsSource = glswGetShader("Simple.Fragment." PEZ_GL_VERSION_TOKEN);
    PezCheckCondition(vsSource, "Can't find vertex shader.\n");
    PezCheckCondition(fsSource, "Can't find fragment shader.\n");

    vsHandle = glCreateShader(GL_VERTEX_SHADER);
    fsHandle = glCreateShader(GL_FRAGMENT_SHADER);

    // PezCheckCondition(!(err = glGetError()), "Error before: %#06x\n", err);

    glShaderSource(vsHandle, 1, &vsSource, 0);
    glCompileShader(vsHandle);
    glGetShaderiv(vsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(vsHandle, sizeof(compilerSpew), 0, compilerSpew);
    printf("compiler: %s\n", compilerSpew);
    PezCheckCondition(compileSuccess, compilerSpew);

    glShaderSource(fsHandle, 1, &fsSource, 0);
    glCompileShader(fsHandle);
    glGetShaderiv(fsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(fsHandle, sizeof(compilerSpew), 0, compilerSpew);
    printf("compiler: %s\n", compilerSpew);
    PezCheckCondition(compileSuccess, compilerSpew);

    programHandle = glCreateProgram();
    glAttachShader(programHandle, vsHandle);
    glAttachShader(programHandle, fsHandle);
    glBindAttribLocation(programHandle, PositionSlot, "Position");
    glBindAttribLocation(programHandle, ColorSlot, "InColor");
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    glGetProgramInfoLog(programHandle, sizeof(compilerSpew), 0, compilerSpew);
    printf("linker: %s\n", compilerSpew);
    PezCheckCondition(linkSuccess, compilerSpew);

    glUseProgram(programHandle);
    // PezCheckCondition(!(err = glGetError()), "Error using program: %#06x\n", err);
}
