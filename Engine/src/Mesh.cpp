#include "Mesh.h"
#include "SDL.h"
#include "GL/glew.h"
#include "Globals.h"
#include "DebugLeaks.h"
#include <vector>
#include <string>

Mesh::Mesh() :
    Component(Component::Type::Mesh, gameObject), Vertices(), Indices(), name()
{
   
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const char* name) : 
    Component(Component::Type::Mesh, gameObject), Vertices(vertices), Indices(indices), name(name)
{
    vao = vbo = ebo = 0;

    numVertices = vertices.size();
    numIndices = indices.size();
    model = float4x4::identity;

    setupMesh();
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

void Mesh::InitMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const char* name)
{
    Vertices = vertices;
    Indices = indices;
    this->name = name;

    vao = vbo = ebo = 0;

    numVertices = vertices.size();
    numIndices = indices.size();
    model = float4x4::identity;

    setupMesh();
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
