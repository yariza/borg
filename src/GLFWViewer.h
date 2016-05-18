#ifndef GLFW_VIEWER_H
#define GLFW_VIEWER_H

// Libraries
#include "Shader.h"
#include "Camera.h"
// #include "maths/Vector2.h"
#include <glm/glm.hpp>
#include <string>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace borg {

// Class Renderer
class GLFWViewer {

    private:
        // -------------------- Attributes -------------------- //

        // Camera

        // Center of the scene
        glm::vec3 m_centerScene;

        // Last mouse coordinates on the windows
        int m_lastMouseX, m_lastMouseY;

        // Last point computed on a sphere (for camera rotation)
        glm::vec3 m_lastPointOnSphere;

        // True if the last point computed on a sphere (for camera rotation) is valid
        bool m_isLastPointOnSphereValid;

        // -------------------- Methods -------------------- //

        bool mapMouseCoordinatesToSphere(int xMouse, int yMouse, glm::vec3& spherePoint) const;

    protected:
        Camera m_camera;

        GLFWwindow *m_window;
        int m_frameBufferWidth, m_frameBufferHeight;
        int m_windowWidth, m_windowHeight;
        double m_time;
        double m_delta;

    public:

        // -------------------- Methods -------------------- //

        // Constructor
        GLFWViewer();

        // Destructor
        virtual ~GLFWViewer();

        GLFWwindow* getWindow();

        // Initialize the viewer
        bool init(const std::string& windowsTitle,
                  const int width, const int height);

        void mainLoop();

        virtual void setup();
        virtual void reshape(int width, int height);
        virtual void mouseButton(int button, int action, int mods);
        virtual void mouseMotion(double xMouse, double yMouse);
        virtual void mouseScroll(double xOffset, double yOffset);
        virtual void keyboard(int key, int scancode, int action, int mods);
        virtual void update();
        virtual void render();

        // Set the scene position (where the camera needs to look at)
        void setScenePosition(const glm::vec3& position, float sceneRadius);

        // Set the camera so that we can view the whole scene
        void resetCameraToViewAll();

        // Zoom the camera
        void zoom(int xMouse, int yMouse);

        // Translate the camera
        void translate(int xMouse, int yMouse);

        // Rotate the camera
        void rotate(int xMouse, int yMouse);

        // Get the camera
        Camera& getCamera();

        void getWindowSize(int &width, int &height);
        void setWindowSize(int width, int height);
};

// Set the scene position (where the camera needs to look at)
inline void GLFWViewer::setScenePosition(const glm::vec3& position, float sceneRadius) {

    // Set the position and radius of the scene
    m_centerScene = position;
    m_camera.setSceneRadius(sceneRadius);

    // Reset the camera position and zoom in order to view all the scene
    resetCameraToViewAll();
}

// Get the camera
inline Camera& GLFWViewer::getCamera() {
   return m_camera;
}

}
#endif
