#pragma once
#include "MathGeoLib/Geometry/Frustum.h"
#include <vector>
#include <string>

struct Vertex {
    float3 Position;
    float2 TexCoords;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<unsigned int> textureId, const char* name);
    ~Mesh();
    void Draw(const unsigned program, const float4x4& proj, const float4x4& view);

    const unsigned int GetNumVertices() { return numVertices; };
    const unsigned int GetNumIndices() { return numIndices; };
    const std::vector<unsigned int> GetTexturesIds() { return TexturesIds; };
    const char* GetName() { return name; }

private:
    std::vector<Vertex>       Vertices;
    std::vector<unsigned int> Indices;
    std::vector<unsigned int> TexturesIds;
    unsigned int vao, vbo, ebo;
    unsigned int numVertices, numIndices;
    const char* name;

    void setupMesh();
};

