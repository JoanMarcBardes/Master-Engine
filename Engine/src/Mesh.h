#pragma once
#include "Libraries/MathGeoLib/Geometry/Frustum.h"
#include <vector>
#include <string>

struct Vertex {
    float3 Position;
    float3 Normal;
    float2 TexCoords;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const char* name);
    ~Mesh();
    void Draw();

    const unsigned int GetNumVertices() { return numVertices; };
    const unsigned int GetNumIndices() { return numIndices; };
    const char* GetName() { return name; }
    const float4x4 GetModel() { return model; }
    const float3 GetMin() { return min; }
    const float3 GetMax() { return max; }

private:
    std::vector<Vertex>       Vertices;
    std::vector<unsigned int> Indices;
    unsigned int vao, vbo, ebo;
    unsigned int numVertices, numIndices;
    const char* name;
    float4x4 model = float4x4::identity;;

    float3 min;
    float3 max;

    void setupMesh();
    void CalculateMinMax();

};
