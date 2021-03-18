#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Texture {
public:
    Texture() : _id(0) {}
    Texture(GLuint id) : _id(id) {}
    virtual ~Texture() = 0 {}
    GLuint GetID() const { return _id; }

protected:
    GLuint _id;
};