#include "TriangleMesh.h"


std::vector<DrawTriangle> TriangleMesh::GetDrawTriangles() const {
    std::vector<DrawTriangle> res;
    for (auto& f : _faces) {
        DrawTriangle triangle;
        for (int i = 0; i < 3; i++) {
            triangle.V[i] = _vertices[f.VertexID[i]];
        }
        res.push_back(triangle);
    }
    return res;
}