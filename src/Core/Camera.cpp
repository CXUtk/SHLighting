#include "Camera.h"

Camera::Camera(glm::vec3 eyePos, float fov, float aspect, float zNear, float zFar)
    : _eyePos(eyePos), _fov(fov), _aspect(aspect), _zNear(zNear), _zFar(zFar) {
    _front = glm::vec3(0, 0, -1);
    _up = glm::vec3(0, 1, 0);
    _right = glm::vec3(1, 0, 0);
}

glm::mat4 Camera::getViewTransform() const {
    return glm::lookAt(_eyePos, _eyePos + _front, _up);
}

glm::mat4 Camera::getProjectTransform() const {
    return glm::perspective(_fov, _aspect, _zNear, _zFar);
}

glm::vec3 Camera::GetDir(float x, float y) {
    x = x * 2 - 1;
    y = y * 2 - 1;
    auto H = _zNear * tan(_fov / 2);
    auto W = H * _aspect;

    x *= W, y *= H;
    return glm::normalize(_right * x + _up * y + _front * _zNear);
}

void Camera::SetRotation(glm::vec2 rot) {
    auto front = glm::vec3(0, 0, -1);
    auto up = glm::vec3(0, 1, 0);
    auto left = glm::normalize(glm::cross(front, up));

    glm::mat4 rMatrix = glm::identity<glm::mat4>();
    rMatrix = glm::rotate(rMatrix, -rot.y, left);
    rMatrix = glm::rotate(rMatrix, rot.x, up);


    _front = glm::vec4(front, 0) * rMatrix;
    _up = glm::vec4(up, 0) * rMatrix;
    _right = glm::vec4(left, 0) * rMatrix;
}
