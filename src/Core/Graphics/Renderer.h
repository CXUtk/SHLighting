#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "RenderUtils.h"
#include "Shaders/ShaderManager.h"
#include <Core/Textures/CubeTexture.h>
#include <algorithm>

class Renderer {
public:
    Renderer();
    ~Renderer();
    void DrawLines(const std::vector<DrawSegment>& lines, const glm::vec3& color, float width);
    void DrawLightedTriangles(const std::vector<DrawTriangle>& triangles, const glm::vec3& color, const glm::vec3& lightDir, const glm::vec3& eyePos);
    void DrawTriangles(const std::vector<DrawTriangle>& triangles, const glm::vec4& color);
    void DrawTrianglesCubemap(const std::vector<DrawTriangle>& triangles, const std::shared_ptr<CubeTexture>& cubemap, glm::vec3 viewPos);
    void DrawCircles(const std::vector<DrawCircle>& circles, const glm::vec4& color);
    void DrawSkyBox(const std::vector<DrawTriangle>& triangles, const std::shared_ptr<CubeTexture>& cubemap);
    void DrawTrianglesSH(const std::vector<DrawTriangle>& triangles);

    void Begin(glm::mat4 projection, glm::mat4 view);
    void End();
private:

    std::shared_ptr<ShaderManager> _shaderManager;
    glm::mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;

    void init_lineBuffer();
    void init_triangleBuffer();
};
