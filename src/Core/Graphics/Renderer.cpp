#include "Renderer.h"
#include <SHLighting.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Core/Textures/CubeTexture.h>

static GLuint _vaoLine, _vboLine;
static GLuint _vaoTriangle, _vboTriangle;
static constexpr int BUFFER_SIZE = 1024;
static constexpr int BUFFER_SIZE_T = 9999;

Renderer::Renderer() {
    _shaderManager = std::make_shared<ShaderManager>();

    //_viewMatrix = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    //_projectionMatrix = glm::perspective(glm::pi<float>() / 2.f, 800.0f / 600.0f, 1.f, 100.f);
    init_lineBuffer();
    init_triangleBuffer();
}

Renderer::~Renderer() {
}

void Renderer::DrawLines(const std::vector<DrawSegment>& lines, const glm::vec3& color, float width) {
    glLineWidth(width);
    auto model = glm::identity<glm::mat4>();
    auto shader = _shaderManager->getShaderData("line_draw");
    shader->apply();
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "projection"), 1, false, glm::value_ptr(_projectionMatrix * _viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"), 1, false, glm::value_ptr(model));
    glUniform4f(glGetUniformLocation(shader->getID(), "uColor"), color.r, color.g, color.b, 1.0f);


    int sz = lines.size();
    glBindVertexArray(_vaoLine);
    // 以 BUFFER_SIZE 个点为单位，分批绘制线段
    for (int i = 0; i < sz; i += BUFFER_SIZE) {
        glBindBuffer(GL_ARRAY_BUFFER, _vboLine);
        int count = std::min(sz, i + BUFFER_SIZE) - i;
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DrawSegment) * count, lines.data() + i);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_LINES, 0, count * 2);
    }

    glBindVertexArray(0);
}

void Renderer::DrawLightedTriangles(const std::vector<DrawTriangle>& triangles, const glm::vec3& color, const glm::vec3& lightDir, const glm::vec3& eyePos) {
    auto model = glm::identity<glm::mat4>();


    auto shader = _shaderManager->getShaderData("light_draw");
    shader->apply();
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "projection"), 1, false, glm::value_ptr(_projectionMatrix * _viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"), 1, false, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(shader->getID(), "uColor"), color.r, color.g, color.b);
    glUniform3f(glGetUniformLocation(shader->getID(), "uLightDir"), lightDir.x, lightDir.y, lightDir.z);
    glUniform3f(glGetUniformLocation(shader->getID(), "uEyePos"), eyePos.x, eyePos.y, eyePos.z);

    glUniform3f(glGetUniformLocation(shader->getID(), "ambientColor"), 0.3f, 0.3f, 0.3f);
    glUniform3f(glGetUniformLocation(shader->getID(), "diffuseColor"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(shader->getID(), "specularColor"), 0.7f, 0.7f, 0.7f);


    int sz = triangles.size();
    glBindVertexArray(_vaoTriangle);
    // 以 BUFFER_SIZE_T 个顶点为单位，分批绘制三角形
    for (int i = 0; i < sz; i += BUFFER_SIZE_T / 3) {
        glBindBuffer(GL_ARRAY_BUFFER, _vboTriangle);
        int count = std::min(sz, i + BUFFER_SIZE_T / 3) - i;
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DrawTriangle) * count, triangles.data() + i);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, count * 3);
    }

    glBindVertexArray(0);
}

void Renderer::DrawTriangles(const std::vector<DrawTriangle>& triangles, const glm::vec4& color) {
    auto model = glm::identity<glm::mat4>();


    auto shader = _shaderManager->getShaderData("triangle_draw");
    shader->apply();
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "projection"), 1, false, glm::value_ptr(_projectionMatrix * _viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"), 1, false, glm::value_ptr(model));
    glUniform4f(glGetUniformLocation(shader->getID(), "uColor"), color.r, color.g, color.b, color.w);


    int sz = triangles.size();
    glBindVertexArray(_vaoTriangle);
    // 以 BUFFER_SIZE_T 个顶点为单位，分批绘制三角形
    for (int i = 0; i < sz; i += BUFFER_SIZE_T / 3) {
        glBindBuffer(GL_ARRAY_BUFFER, _vboTriangle);
        int count = std::min(sz, i + BUFFER_SIZE_T / 3) - i;
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DrawTriangle) * count, triangles.data() + i);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, count * 3);
    }

    glBindVertexArray(0);
}

void Renderer::DrawTrianglesCubemap(const std::vector<DrawTriangle>& triangles, const std::shared_ptr<CubeTexture>& cubemap, glm::vec3 viewPos) {
    auto model = glm::identity<glm::mat4>();


    auto shader = _shaderManager->getShaderData("triangle_draw_cube");
    shader->apply();
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "projection"), 1, false, glm::value_ptr(_projectionMatrix * _viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"), 1, false, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shader->getID(), "viewPos"), 1, glm::value_ptr(viewPos));

    glBindVertexArray(_vaoTriangle);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->GetID());
    glUniform1i(glGetUniformLocation(shader->getID(), "cubemap"), 0);

    int sz = triangles.size();
    // 以 BUFFER_SIZE_T 个顶点为单位，分批绘制三角形
    for (int i = 0; i < sz; i += BUFFER_SIZE_T / 3) {
        glBindBuffer(GL_ARRAY_BUFFER, _vboTriangle);
        int count = std::min(sz, i + BUFFER_SIZE_T / 3) - i;
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DrawTriangle) * count, triangles.data() + i);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, count * 3);
    }

    glBindVertexArray(0);
}

void Renderer::DrawCircles(const std::vector<DrawCircle>& circles, const glm::vec4& color) {
    std::vector<DrawTriangle> trianglesQuads;

}

void Renderer::DrawSkyBox(const std::vector<DrawTriangle>& triangles, const std::shared_ptr<CubeTexture>& cubemap) {
    auto model = glm::identity<glm::mat4>();


    auto shader = _shaderManager->getShaderData("skybox");
    shader->apply();
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "projection"), 1, false, glm::value_ptr(_projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "view"), 1, false, glm::value_ptr(_viewMatrix));

    glBindVertexArray(_vaoTriangle);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->GetID());
    glUniform1i(glGetUniformLocation(shader->getID(), "cubemap"), 0);

    int sz = triangles.size();
    // 以 BUFFER_SIZE_T 个顶点为单位，分批绘制三角形
    for (int i = 0; i < sz; i += BUFFER_SIZE_T / 3) {
        glBindBuffer(GL_ARRAY_BUFFER, _vboTriangle);
        int count = std::min(sz, i + BUFFER_SIZE_T / 3) - i;
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DrawTriangle) * count, triangles.data() + i);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, count * 3);
    }

    glBindVertexArray(0);
}

void Renderer::DrawTrianglesSH(const std::vector<DrawTriangle>& triangles) {
    auto model = glm::identity<glm::mat4>();

    //glm::mat4 RMat = glm::mat4(0.009098, -0.004780, 0.024033, -0.014947,
    //    -0.004780, -0.009098, -0.011258, 0.020210,
    //    0.024033, -0.011258, -0.011570, -0.017383,
    //    -0.014947, 0.020210, -0.017383, 0.073787);

    //glm::mat4 GMat = glm::mat4(-0.002331, -0.002184, 0.009201, -0.002846,
    //    -0.002184, 0.002331, -0.009611, 0.017903,
    //    0.009201, -0.009611, -0.007038, -0.009331,
    //    -0.002846, 0.017903, -0.009331, 0.041083);

    //glm::mat4 BMat = glm::mat4(-0.013032, -0.005248, 0.005970, 0.000483,
    //    -0.005248, 0.013032, -0.020370, 0.030949,
    //    0.005970, -0.020370, -0.010948, -0.013784,
    //    0.000483, 0.030949, -0.013784, 0.051648);

    glm::mat4 RMat = glm::mat4(
        0.002602, 0.000858, -0.002529, -0.001215,
        0.000858, -0.002602, -0.002134, 0.006012,
        -0.002529, -0.002134, 0.008727, -0.007814,
        -0.001215, 0.006012, -0.007814, 0.030030
        );
    glm::mat4 GMat = glm::mat4(
        0.003390, 0.000971, -0.001974, -0.002210,
        0.000971, -0.003390, -0.001624, 0.008696,
        -0.001974, -0.001624, 0.007176, -0.008548,
        -0.002210, 0.008696, -0.008548, 0.041760
        );
    glm::mat4 BMat = glm::mat4(
        0.002841, 0.000853, -0.001103, -0.002904,
        0.000853, -0.002841, -0.000948, 0.013175,
        -0.001103, -0.000948, 0.003537, -0.008186,
        -0.002904, 0.013175, -0.008186, 0.054869
        );
    auto shader = _shaderManager->getShaderData("triangle_draw_SH");
    shader->apply();
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "projection"), 1, false, glm::value_ptr(_projectionMatrix * _viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"), 1, false, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "RMat"), 1, false, glm::value_ptr(RMat));
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "GMat"), 1, false, glm::value_ptr(GMat));
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "BMat"), 1, false, glm::value_ptr(BMat));

    glBindVertexArray(_vaoTriangle);


    int sz = triangles.size();
    // 以 BUFFER_SIZE_T 个顶点为单位，分批绘制三角形
    for (int i = 0; i < sz; i += BUFFER_SIZE_T / 3) {
        glBindBuffer(GL_ARRAY_BUFFER, _vboTriangle);
        int count = std::min(sz, i + BUFFER_SIZE_T / 3) - i;
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DrawTriangle) * count, triangles.data() + i);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, count * 3);
    }

    glBindVertexArray(0);
}

void Renderer::Begin(glm::mat4 projection, glm::mat4 view) {
    _projectionMatrix = projection;
    _viewMatrix = view;
}

void Renderer::End() {
}

void Renderer::init_lineBuffer() {
    // 画线的顶点缓存
    glGenVertexArrays(1, &_vaoLine);
    glGenBuffers(1, &_vboLine);

    glBindVertexArray(_vaoLine);
    glBindBuffer(GL_ARRAY_BUFFER, _vboLine);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DrawSegment) * BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::init_triangleBuffer() {
    // 画三角形面的顶点缓存
    glGenVertexArrays(1, &_vaoTriangle);
    glGenBuffers(1, &_vboTriangle);

    glBindVertexArray(_vaoTriangle);
    glBindBuffer(GL_ARRAY_BUFFER, _vboTriangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * BUFFER_SIZE_T, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(glm::vec3) * 2));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
