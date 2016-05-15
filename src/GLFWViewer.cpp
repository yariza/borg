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
    glfwSetCursorPosCallback(m_window, mouseMotionCallback);

    // float radius = 10.0f;
    // Vector3 center(0.0, 0.0, 0.0);

    // setScenePosition(center, radius);

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
    // mCamera.setDimensions(width, height);
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
        mLastMouseX = x;
        mLastMouseY = y;
        // mIsLastPointOnSphereValid = mapMouseCoordinatesToSphere(x, y, mLastPointOnSphere);
    }
    else {  // If the mouse button is released
        mIsLastPointOnSphereValid = false;

        // If it is a mouse wheel click event
        if (button == 3) {
            // zoom(0, (int) (y - 0.05f * mCamera.getWidth()));
        }
        else if (button == 4) {
            // zoom(0, (int) (y + 0.05f * mCamera.getHeight()));
        }
    }
}

void GLFWViewer::mouseMotion(double xMouse, double yMouse) {

    // Zoom
    if ((glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
         glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) ||
        (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
         glfwGetKey(m_window, GLFW_KEY_LEFT_ALT))) {
        // zoom(xMouse, yMouse);
    }
    // Translation
    else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS ||
             glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ||
             (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) &&
             glfwGetKey(m_window, GLFW_KEY_LEFT_ALT))) {
        // translate(xMouse, yMouse);
    }
    // Rotation
    else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT)) {
        // rotate(xMouse, yMouse);
    }

    // Remember the mouse position
    mLastMouseX = xMouse;
    mLastMouseY = yMouse;
    // mIsLastPointOnSphereValid = mapMouseCoordinatesToSphere(xMouse, yMouse, mLastPointOnSphere);
}

void GLFWViewer::update() {
}

void GLFWViewer::render() {
}

void GLFWViewer::mainLoop() {

    setup();

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
// void GLFWViewer::resetCameraToViewAll() {

//     // Move the camera to the origin of the scene
//     mCamera.translateWorld(-mCamera.getOrigin());

//     // Move the camera to the center of the scene
//     mCamera.translateWorld(mCenterScene);

//     // Set the zoom of the camera so that the scene center is
//     // in negative view direction of the camera
//     mCamera.setZoom(1.0);
// }

// Map the mouse x,y coordinates to a point on a sphere
// bool GLFWViewer::mapMouseCoordinatesToSphere(int xMouse, int yMouse, Vector3& spherePoint) const {

//     int width = mCamera.getWidth();
//     int height = mCamera.getHeight();

//     if ((xMouse >= 0) && (xMouse <= width) && (yMouse >= 0) && (yMouse <= height)) {
//         float x = float(xMouse - 0.5f * width) / float(width);
//         float y = float(0.5f * height - yMouse) / float(height);
//         float sinx = sin(PIE * x * 0.5f);
//         float siny = sin(PIE * y * 0.5f);
//         float sinx2siny2 = sinx * sinx + siny * siny;

//         // Compute the point on the sphere
//         spherePoint.x = sinx;
//         spherePoint.y = siny;
//         spherePoint.z = (sinx2siny2 < 1.0) ? sqrt(1.0f - sinx2siny2) : 0.0f;

//         return true;
//     }

//     return false;
// }

// Zoom the camera
// void GLFWViewer::zoom(int xMouse, int yMouse) {
//     float dy = static_cast<float>(yMouse - mLastMouseY);
//     float h = static_cast<float>(mCamera.getHeight());

//     // Zoom the camera
//     mCamera.setZoom(-dy / h);
// }

// Translate the camera
// void GLFWViewer::translate(int xMouse, int yMouse) {
//    float dx = static_cast<float>(xMouse - mLastMouseX);
//    float dy = static_cast<float>(yMouse - mLastMouseY);

//    // Translate the camera
//    mCamera.translateCamera(-dx / float(mCamera.getWidth()),
//                            -dy / float(mCamera.getHeight()), mCenterScene);
// }

// Rotate the camera
// void GLFWViewer::rotate(int xMouse, int yMouse) {
//     if (mIsLastPointOnSphereValid) {

//         Vector3 newPoint3D;
//         bool isNewPointOK = mapMouseCoordinatesToSphere(xMouse, yMouse, newPoint3D);

//         if (isNewPointOK) {
//             Vector3 axis = mLastPointOnSphere.cross(newPoint3D);
//             float cosAngle = mLastPointOnSphere.dot(newPoint3D);

//             float epsilon = std::numeric_limits<float>::epsilon();
//             if (fabs(cosAngle) < 1.0f && axis.length() > epsilon) {
//                 axis.normalize();
//                 float angle = 2.0f * acos(cosAngle);

//                 // Rotate the camera around the center of the scene>
//                 mCamera.rotateAroundLocalPoint(axis, -angle, mCenterScene);
//             }
//         }
//     }
// }

void GLFWViewer::getWindowSize(int &width, int &height) {
    glfwGetFramebufferSize(m_window, &width, &height);
}

void GLFWViewer::setWindowSize(int width, int height) {
    glfwSetWindowSize(m_window, width, height);
    reshape(width, height);
}
