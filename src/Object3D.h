/********************************************************************************
* OpenGL-Framework                                                              *
* Copyright (c) 2013 Daniel Chappuis                                            *
*********************************************************************************
*                                                                               *
* This software is provided 'as-is', without any express or implied warranty.   *
* In no event will the authors be held liable for any damages arising from the  *
* use of this software.                                                         *
*                                                                               *
* Permission is granted to anyone to use this software for any purpose,         *
* including commercial applications, and to alter it and redistribute it        *
* freely, subject to the following restrictions:                                *
*                                                                               *
* 1. The origin of this software must not be misrepresented; you must not claim *
*    that you wrote the original software. If you use this software in a        *
*    product, an acknowledgment in the product documentation would be           *
*    appreciated but is not required.                                           *
*                                                                               *
* 2. Altered source versions must be plainly marked as such, and must not be    *
*    misrepresented as being the original software.                             *
*                                                                               *
* 3. This notice may not be removed or altered from any source distribution.    *
*                                                                               *
********************************************************************************/

#ifndef OBJECT3D_H
#define OBJECT3D_H

// Libraries
// #include <glm.hpp>
#include <glm/gtx/transform.hpp>

namespace borg {

// Class Object3D
// This class represent a generic 3D object on the scene.
class Object3D {

    protected:

        // -------------------- Attributes -------------------- //

        // Transformation matrix that convert local-space
        // coordinates to world-space coordinates
        glm::mat4 mTransformMatrix;

    public:

        // -------------------- Methods -------------------- //

        // Constructor
        Object3D();

        // Destructor
        virtual ~Object3D();

        // Return the transform matrix
        const glm::mat4& getTransformMatrix() const;

        // Set to the identity transform
        void setToIdentity();

        // Return the origin of object in world-space
        glm::vec3 getOrigin() const;

        // Translate the object in world-space
        void translateWorld(const glm::vec3& v);

        // Translate the object in local-space
        void translateLocal(const glm::vec3& v);

        // Rotate the object in world-space
        void rotateWorld(const glm::vec3& axis, float angle);

        // Rotate the object in local-space
        void rotateLocal(const glm::vec3& axis, float angle);

        // Rotate around a world-space point
        void rotateAroundWorldPoint(const glm::vec3& axis, float angle, const glm::vec3& point);

        // Rotate around a local-space point
        void rotateAroundLocalPoint(const glm::vec3& axis, float angle, const glm::vec3& worldPoint);
};

// Return the transform matrix
inline const glm::mat4& Object3D::getTransformMatrix() const {
    return mTransformMatrix;
}

// Set to the identity transform
inline void Object3D::setToIdentity() {
    mTransformMatrix = glm::mat4();
}

 // Return the origin of object in world-space
inline glm::vec3 Object3D::getOrigin() const {
    return glm::vec3(mTransformMatrix * glm::vec4(0.0, 0.0, 0.0, 1.0));
}

// Translate the object in world-space
inline void Object3D::translateWorld(const glm::vec3& v) {
    mTransformMatrix = glm::translate(v) * mTransformMatrix;
}

// Translate the object in local-space
inline void Object3D::translateLocal(const glm::vec3& v) {
    mTransformMatrix = mTransformMatrix * glm::translate(v);
}

// Rotate the object in world-space
inline void Object3D::rotateWorld(const glm::vec3& axis, float angle) {
    mTransformMatrix = glm::rotate(angle, axis) * mTransformMatrix;
}

// Rotate the object in local-space
inline void Object3D::rotateLocal(const glm::vec3& axis, float angle) {
    mTransformMatrix = mTransformMatrix * glm::rotate(angle, axis);
}

// Rotate the object around a world-space point
inline void Object3D::rotateAroundWorldPoint(const glm::vec3& axis, float angle,
                                             const glm::vec3& worldPoint) {
    mTransformMatrix = glm::translate(worldPoint) * glm::rotate(angle, axis)
                       * glm::translate(-worldPoint) * mTransformMatrix;
}

// Rotate the object around a local-space point
inline void Object3D::rotateAroundLocalPoint(const glm::vec3& axis, float angle,
                                             const glm::vec3& worldPoint) {

    // Convert the world point into the local coordinate system
    glm::vec3 localPoint = glm::vec3(glm::inverse(mTransformMatrix) * glm::vec4(worldPoint, 1.0));

    mTransformMatrix = mTransformMatrix * glm::translate(localPoint)
                       * glm::rotate(angle, axis)
                       * glm::translate(-localPoint);
}

}

#endif
