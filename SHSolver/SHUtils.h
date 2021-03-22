#pragma once
#include <random>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>

inline float GetRandomFloat() {
    static std::mt19937 mt;
    return mt() / (float)mt.max();
}

inline glm::vec3 GetUniformRandSphere() {
    auto v = 2 * GetRandomFloat() - 1;
    auto theta = GetRandomFloat() * glm::two_pi<float>();
    auto r = std::sqrt(1 - v * v);
    return glm::vec3(r * std::cos(theta), v, r * std::sin(theta));
}
