#include "Mesh.h"
#include "SDL.h"
#include "GL/glew.h"
#include "Globals.h"
#include "DebugLeaks.h"
#include <vector>
#include <string>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const char* _name)
{
    Vertices = vertices;
    Indices = indices;

    vao = vbo = ebo = 0;

    numVertices = vertices.size();
    numIndices = indices.size();
    name = _name;
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
    // vertex texture coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::Draw(const unsigned program, const float4x4& proj, const float4x4& view, const float4x4& _model, std::vector<unsigned int>& texturesList)
{
    model = _model;
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
    glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

    glActiveTexture(GL_TEXTURE0);
    for (unsigned int i = 0; i < texturesList.size(); ++i) {
        glBindTexture(GL_TEXTURE_2D, texturesList[i]);
        glUniform1i(glGetUniformLocation(program, "texture"+i), i);
    }

    // draw mesh
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::CalculateMinMax()
{
    min = Vertices[0].Position;
    max = Vertices[0].Position;

    for (int i = 1; i < Vertices.size(); ++i)
    {
        //min
        if (Vertices[i].Position.x < min.x)
            min.x = Vertices[i].Position.x;
        if (Vertices[i].Position.y < min.y)
            min.y = Vertices[i].Position.y;
        if (Vertices[i].Position.z < min.z)
            min.z = Vertices[i].Position.z;

        //max
        if (Vertices[i].Position.x > max.x)
            max.x = Vertices[i].Position.x;
        if (Vertices[i].Position.y > max.y)
            max.y = Vertices[i].Position.y;
        if (Vertices[i].Position.z > max.z)
            max.z = Vertices[i].Position.z;
    }
}
