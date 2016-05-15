#include "Fluid.h"
#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

static GLuint QuadVao;
static GLuint VisualizeProgram;
static Slab Velocity, Density, Pressure, Temperature;
static Surface Divergence, Obstacles, HiresObstacles;

borg::GLFWViewer* g_viewer;

class MyViewer: public borg::GLFWViewer {
public:
    void setup() {
        int w = GridWidth;
        int h = GridHeight;
        Velocity = CreateSlab(w, h, 2);
        Density = CreateSlab(w, h, 1);
        Pressure = CreateSlab(w, h, 1);
        Temperature = CreateSlab(w, h, 1);
        Divergence = CreateSurface(w, h, 3);
        InitSlabOps();
        VisualizeProgram = CreateProgram("Fluid.Vertex.GL3", "Fluid.Visualize.GL3");

        Obstacles = CreateSurface(w, h, 3);
        CreateObstacles(Obstacles, w, h);

        w = ViewportWidth * 2;
        h = ViewportHeight * 2;
        HiresObstacles = CreateSurface(w, h, 1);
        CreateObstacles(HiresObstacles, w, h);

        QuadVao = CreateQuad();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ClearSurface(Temperature.Ping, AmbientTemperature);
    }

    void update() {
        glViewport(0, 0, GridWidth, GridHeight);

        Advect(Velocity.Ping, Velocity.Ping, Obstacles, Velocity.Pong, VelocityDissipation);
        SwapSurfaces(&Velocity);

        Advect(Velocity.Ping, Temperature.Ping, Obstacles, Temperature.Pong, TemperatureDissipation);
        SwapSurfaces(&Temperature);

        Advect(Velocity.Ping, Density.Ping, Obstacles, Density.Pong, DensityDissipation);
        SwapSurfaces(&Density);

        ApplyBuoyancy(Velocity.Ping, Temperature.Ping, Density.Ping, Velocity.Pong);
        SwapSurfaces(&Velocity);

        ApplyImpulse(Temperature.Ping, ImpulsePosition, ImpulseTemperature);
        ApplyImpulse(Density.Ping, ImpulsePosition, ImpulseDensity);

        ComputeDivergence(Velocity.Ping, Obstacles, Divergence);
        ClearSurface(Pressure.Ping, 0);

        for (int i = 0; i < NumJacobiIterations; ++i) {
            Jacobi(Pressure.Ping, Divergence, Obstacles, Pressure.Pong);
            SwapSurfaces(&Pressure);
        }

        SubtractGradient(Velocity.Ping, Pressure.Ping, Obstacles, Velocity.Pong);
        SwapSurfaces(&Velocity);
    }

    void render() {
        // Bind visualization shader and set up blend state:
        glUseProgram(VisualizeProgram);
        GLint fillColor = glGetUniformLocation(VisualizeProgram, "FillColor");
        GLint scale = glGetUniformLocation(VisualizeProgram, "Scale");
        glEnable(GL_BLEND);

        // Set render target to the backbuffer:
        glViewport(0, 0, ViewportWidth, ViewportHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw ink:
        glBindTexture(GL_TEXTURE_2D, Density.Ping.TextureHandle);
        glUniform3f(fillColor, 1, 1, 1);
        glUniform2f(scale, 1.0f / ViewportWidth, 1.0f / ViewportHeight);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Draw obstacles:
        glBindTexture(GL_TEXTURE_2D, HiresObstacles.TextureHandle);
        glUniform3f(fillColor, 0.125f, 0.4f, 0.75f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Disable blending:
        glDisable(GL_BLEND);
    }
};

int main(void)
{
    g_viewer = new MyViewer();
    bool initOK = g_viewer->init("Fluid Demo", ViewportWidth, ViewportHeight);
    if (!initOK) {
        std::cout << "Error in init" << std::endl;
        exit(EXIT_FAILURE);
    }

    // dirty hack that doesn't work
    // int fbWidth, fbHeight;
    // glfwGetFramebufferSize(g_viewer->getWindow(), &fbWidth, &fbHeight);
    // if (fbWidth != ViewportWidth) {
    //     ViewportWidth = fbWidth;
    //     ViewportHeight = fbHeight;
    // }

    g_viewer->mainLoop();
}
