#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Camera {
public:
    Camera() = default;
    Camera(glm::vec3 eyePos, float fov, float aspect, float zNear, float zFar);
    ~Camera() = default;

    glm::mat4 getViewTransform() const;
    glm::mat4 getProjectTransform() const;

    void SetEyePos(glm::vec3 pos) { _eyePos = pos; }
    glm::vec3 GetEyePos() const { return _eyePos; }

    glm::vec3 GetDir(float x, float y);

    void SetRotation(glm::vec2 rot);
    void MoveFront(float delta) { _eyePos += _front * delta; }
    void MoveRight(float delta) { _eyePos += _right * delta; }

private:
    glm::vec3 _eyePos, _front, _up, _right;
    float _fov, _aspect, _zNear, _zFar;
};
