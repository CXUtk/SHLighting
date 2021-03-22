#include "SHSampler.h"

SHSampler::SHSampler(float* data, int width) : _width(width) {
    _data = new glm::vec3[width * width];
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < 3; k++) {
                _data[i * width + j][k] = data[(i * width + j) * 3 + k];
            }
        }
    }
}

SHSampler::~SHSampler() {
    delete[] _data;
}

glm::vec3 SHSampler::Sample(glm::vec2 uv) const {
    int r = (int)(glm::clamp(uv.x, 0.f, 0.9999f) * _width);
    int c = (int)(glm::clamp(uv.y, 0.f, 0.9999f) * _width);
    return _data[r * _width + c];
}
