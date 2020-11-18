#pragma once
#include "MathGeoLib/Geometry/Frustum.h"
#include <vector>

struct Vertex {
    float3 Position;
    float2 TexCoords;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<unsigned int> textureId);
    ~Mesh();
    void Draw(const unsigned program, const float4x4& proj, const float4x4& view);

private:
    std::vector<Vertex>       Vertices;
    std::vector<unsigned int> Indices;
    std::vector<unsigned int> TexturesIds;
    unsigned int vao, vbo, ebo;
    unsigned int numVertices, numIndices;

    void setupMesh();
};

