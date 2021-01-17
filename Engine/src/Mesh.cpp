#include "Mesh.h"
#include "SDL.h"
#include "GL/glew.h"
#include "Globals.h"
#include "DebugLeaks.h"
#include <vector>
#include <string>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const char* name) : 
    Component(Component::Type::Mesh, gameObject), Vertices(vertices), Indices(indices), name(name)
{
    vao = vbo = ebo = 0;

    numVertices = vertices.size();
    numIndices = indices.size();
    model = float4x4::identity;

    setupMesh();
    CalculateMinMax();
}

Mesh::~Mesh()
{
    LOG("Destroy Mesh");
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    Vertices.clear();
    Indices.clear();
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::Draw()
{
    // draw mesh
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

float3* Mesh::GetPoints()
{
    math::float3 *points = new math::float3[numVertices];

    for (int i = 0; i < Vertices.size(); ++i)
    {
        points[i] = Vertices[i].Position;
    }

    return points;
}

void Mesh::CalculateMinMax()
{
    //min = Vertices[0].Position;
    //max = Vertices[0].Position;

    //for (int i = 1; i < Vertices.size(); ++i)
    //{
    //    //min
    //    if (Vertices[i].Position.x < min.x)
    //        min.x = Vertices[i].Position.x;
    //    if (Vertices[i].Position.y < min.y)
    //        min.y = Vertices[i].Position.y;
    //    if (Vertices[i].Position.z < min.z)
    //        min.z = Vertices[i].Position.z;

    //    //max
    //    if (Vertices[i].Position.x > max.x)
    //        max.x = Vertices[i].Position.x;
    //    if (Vertices[i].Position.y > max.y)
    //        max.y = Vertices[i].Position.y;
    //    if (Vertices[i].Position.z > max.z)
    //        max.z = Vertices[i].Position.z;
    //}
}
