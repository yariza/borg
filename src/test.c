#include <gl_core_4_1.h>
#include <pez.h>
#include <glsw.h>

GLuint shaderProgram;
GLuint vertexArrayObject;
GLuint vertexBuffer;

GLint positionUniform;
GLint colourAttribute;
GLint positionAttribute;

static void BuildGeometry();
static void LoadEffect();

enum { PositionSlot, ColorSlot };

void PezHandleMouse(int x, int y, int action) { }

void PezUpdate(unsigned int elapsedMilliseconds) { }

void PezRender()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    GLfloat p[]={0,0};
    glUniform2fv(positionUniform, 1, (const GLfloat *)&p);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

const char* PezInitialize(int width, int height)
{
    // 3. Define and compile vertex and fragment shaders
    GLuint  vs;
    GLuint  fs;

    GLint compileSuccess, linkSuccess;
    GLchar compilerSpew[256];

    glswInit();
    glswSetPath("../shader/", ".glsl");
    glswAddDirectiveToken("GL3", "#version 150");

    const char *vss = glswGetShader("test.Vertex.GL3");
    const char *fss = glswGetShader("test.Fragment.GL3");
    PezCheckCondition(vss, "Can't find vertex shader.\n");
    PezCheckCondition(fss, "Can't find fragment shader.\n");

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vss, NULL);
    glCompileShader(vs);

    glGetShaderiv(vs, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(vs, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, compilerSpew);


    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fss, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(fs, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, compilerSpew);


    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glBindFragDataLocation(shaderProgram, 0, "fragColour");
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkSuccess);
    glGetProgramInfoLog(shaderProgram, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(linkSuccess, compilerSpew);


    positionUniform = glGetUniformLocation(shaderProgram, "p");
    colourAttribute = glGetAttribLocation(shaderProgram, "colour");
    positionAttribute = glGetAttribLocation(shaderProgram, "position");
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLfloat vertexData[]= { -0.5,-0.5,0.0,1.0,   1.0,0.0,0.0,1.0,
                            -0.5, 0.5,0.0,1.0,   0.0,1.0,0.0,1.0,
                             0.5, 0.5,0.0,1.0,   0.0,0.0,1.0,1.0,
                             0.5,-0.5,0.0,1.0,   1.0,1.0,1.0,1.0};
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4*8*sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray((GLuint)positionAttribute);
    glEnableVertexAttribArray((GLuint)colourAttribute  );
    glVertexAttribPointer((GLuint)positionAttribute, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
    glVertexAttribPointer((GLuint)colourAttribute  , 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (char*)0+4*sizeof(GLfloat));

    return "Test Frame";
}
