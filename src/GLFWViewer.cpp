#include <glad.h>
#include "GLFWViewer.h"
#include <iostream>
#include <cassert>

using namespace borg;
using namespace std;

static void reshapeCallback(GLFWwindow* window, int width, int height) {
    GLFWViewer* viewer = (GLFWViewer*)glfwGetWindowUserPointer(window);
    viewer->reshape(width, height);
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    GLFWViewer* viewer = (GLFWViewer*)glfwGetWindowUserPointer(window);
    viewer->mouseButton(button, action, mods);
}

static void mouseMotionCallback(GLFWwindow* window, double xMouse, double yMouse) {
    GLFWViewer* viewer = (GLFWViewer*)glfwGetWindowUserPointer(window);
    viewer->mouseMotion(xMouse, yMouse);
}

static void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    GLFWViewer* viewer = (GLFWViewer*)glfwGetWindowUserPointer(window);
    viewer->mouseScroll(xOffset, yOffset);
}

static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    GLFWViewer* viewer = (GLFWViewer*)glfwGetWindowUserPointer(window);
    viewer->keyboard(key, scancode, action, mods);
}

GLFWViewer::GLFWViewer()
: m_window(NULL) {

}

GLFWViewer::~GLFWViewer() {

}

GLFWwindow* GLFWViewer::getWindow() {
    return m_window;
}

bool GLFWViewer::init(const string& windowsTitle,
                      const int width, const int height) {

    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, windowsTitle.c_str(), NULL, NULL);

    if (!m_window) {
        glfwTerminate();
        return false;
    }

    glfwGetFramebufferSize(m_window, &m_frameBufferWidth, &m_frameBufferHeight);
    glfwGetWindowSize(m_window, &m_windowWidth, &m_windowHeight);
    m_time = 0.0;
    m_delta = 0.0;

    glfwSetWindowUserPointer(m_window, (void*)this);
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(m_window, reshapeCallback);
    glfwSetKeyCallback(m_window, keyboardCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetScrollCallback(m_window, mouseScrollCallback);
    glfwSetCursorPosCallback(m_window, mouseMotionCallback);

    float radius = 10.0f;
    glm::vec3 center(0.0, 0.0, 0.0);

    setScenePosition(center, radius);

    if(!gladLoadGL()) {
        cerr << "LoadFunctions failed." << std::endl;
        assert(false);
        return false;
    }

    return true;
}

void GLFWViewer::setup() {
}

void GLFWViewer::reshape(int width, int height) {
    m_camera.setDimensions(width, height);
    m_frameBufferWidth = width;
    m_frameBufferHeight = height;
}

void GLFWViewer::keyboard(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, GL_TRUE);
    }
}

void GLFWViewer::mouseButton(int button, int action, int mods) {

    double x, y;
    glfwGetCursorPos(m_window, &x, &y);

    // If the mouse button is pressed
    if (action == GLFW_PRESS) {
        m_lastMouseX = x;
        m_lastMouseY = y;
        m_isLastPointOnSphereValid = mapMouseCoordinatesToSphere(x, y, m_lastPointOnSphere);
    }
    else {  // If the mouse button is released
        m_isLastPointOnSphereValid = false;

        // If it is a mouse wheel click event
        if (button == 3) {
            zoom(0, (int) (y - 0.05f * m_camera.getWidth()));
        }
        else if (button == 4) {
            zoom(0, (int) (y + 0.05f * m_camera.getHeight()));
        }
    }
}

void GLFWViewer::mouseMotion(double xMouse, double yMouse) {

    // Zoom
    if ((glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
         glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) ||
        (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
         glfwGetKey(m_window, GLFW_KEY_LEFT_ALT))) {
        zoom(xMouse, yMouse);
    }
    // Translation
    else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS ||
             glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ||
             (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) &&
             glfwGetKey(m_window, GLFW_KEY_LEFT_ALT))) {
        translate(xMouse, yMouse);
    }
    // Rotation
    else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT)) {
        rotate(xMouse, yMouse);
    }

    // Remember the mouse position
    m_lastMouseX = xMouse;
    m_lastMouseY = yMouse;
    m_isLastPointOnSphereValid = mapMouseCoordinatesToSphere(xMouse, yMouse, m_lastPointOnSphere);
}

void GLFWViewer::mouseScroll(double xOffset, double yOffset) {
    float h = static_cast<float>(m_camera.getHeight());
    // Zoom the camera
    m_camera.setZoom(-yOffset / h);
}

void GLFWViewer::update() {
}

void GLFWViewer::render() {
}

void GLFWViewer::mainLoop() {

    setup();
    glfwGetFramebufferSize(m_window, &m_frameBufferWidth, &m_frameBufferHeight);
    reshape(m_frameBufferWidth, m_frameBufferHeight);

    while (!glfwWindowShouldClose(m_window)) {
        glfwGetFramebufferSize(m_window, &m_frameBufferWidth, &m_frameBufferHeight);
        glfwGetWindowSize(m_window, &m_windowWidth, &m_windowHeight);
        double oldTime = m_time;
        m_time = glfwGetTime();
        m_delta = m_time - oldTime;

        render();
        update();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

// Set the camera so that we can view the whole scene
void GLFWViewer::resetCameraToViewAll() {

    // Move the camera to the origin of the scene
    m_camera.translateWorld(-m_camera.getOrigin());

    // Move the camera to the center of the scene
    m_camera.translateWorld(m_centerScene);

    // Set the zoom of the camera so that the scene center is
    // in negative view direction of the camera
    m_camera.setZoom(1.0);
}

// Map the mouse x,y coordinates to a point on a sphere
bool GLFWViewer::mapMouseCoordinatesToSphere(int xMouse, int yMouse, glm::vec3& spherePoint) const {

    int width = m_camera.getWidth();
    int height = m_camera.getHeight();

    if ((xMouse >= 0) && (xMouse <= width) && (yMouse >= 0) && (yMouse <= height)) {
        float x = float(xMouse - 0.5f * width) / float(width);
        float y = float(0.5f * height - yMouse) / float(height);
        float sinx = sin(PI * x * 0.5f);
        float siny = sin(PI * y * 0.5f);
        float sinx2siny2 = sinx * sinx + siny * siny;

        // Compute the point on the sphere
        spherePoint.x = sinx;
        spherePoint.y = siny;
        spherePoint.z = (sinx2siny2 < 1.0) ? sqrt(1.0f - sinx2siny2) : 0.0f;

        return true;
    }

    return false;
}

// Zoom the camera
void GLFWViewer::zoom(int xMouse, int yMouse) {
    float dy = static_cast<float>(yMouse - m_lastMouseY);
    float h = static_cast<float>(m_camera.getHeight());

    // Zoom the camera
    m_camera.setZoom(-dy / h);
}

// Translate the camera
void GLFWViewer::translate(int xMouse, int yMouse) {
   float dx = static_cast<float>(xMouse - m_lastMouseX);
   float dy = static_cast<float>(yMouse - m_lastMouseY);

   // Translate the camera
   m_camera.translateCamera(-dx / float(m_camera.getWidth()),
                           -dy / float(m_camera.getHeight()), m_centerScene);
}

// Rotate the camera
void GLFWViewer::rotate(int xMouse, int yMouse) {
    if (m_isLastPointOnSphereValid) {

        glm::vec3 newPoint3D;
        bool isNewPointOK = mapMouseCoordinatesToSphere(xMouse, yMouse, newPoint3D);

        if (isNewPointOK) {
            glm::vec3 axis = glm::cross(m_lastPointOnSphere, newPoint3D);
            float cosAngle = glm::dot(m_lastPointOnSphere, newPoint3D);

            float epsilon = std::numeric_limits<float>::epsilon();
            if (fabs(cosAngle) < 1.0f && axis.length() > epsilon) {
                axis = glm::normalize(axis);
                float angle = 2.0f * acos(cosAngle);

                // Rotate the camera around the center of the scene>
                m_camera.rotateAroundLocalPoint(axis, -angle, m_centerScene);
            }
        }
    }
}

void GLFWViewer::getWindowSize(int &width, int &height) {
    glfwGetFramebufferSize(m_window, &width, &height);
}

void GLFWViewer::setWindowSize(int width, int height) {
    glfwSetWindowSize(m_window, width, height);
    reshape(width, height);
}
