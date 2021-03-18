﻿#pragma once
#include <vector>
#include "VertexData.h"
#include <Core/Graphics/RenderUtils.h>

class TriangleMesh {
public:
    TriangleMesh() {}

    void SetVertices(const std::vector<VertexData>& vertices) {
        _vertices = vertices;
    }
    void SetFaces(const std::vector<TriangleFaceIndex>& faces) {
        _faces = faces;
    }

    std::vector<VertexData> GetVertices() const { return _vertices; }
    std::vector<TriangleFaceIndex> GetFaces() const { return _faces; }

    std::vector<DrawTriangle> GetDrawTriangles() const;

private:
    std::vector<VertexData> _vertices;
    std::vector<TriangleFaceIndex> _faces;
};
