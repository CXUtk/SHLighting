#pragma once

#include "Texture.h"
#include <vector>
#include <string>
class CubeTexture : public Texture {
public:
    CubeTexture(const std::vector<std::string>& faces);

};