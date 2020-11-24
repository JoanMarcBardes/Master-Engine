#include "Mesh.h"
#include "SDL.h"
#include "GL/glew.h"
#include "DebugLeaks.h"
#include <vector>
#include <string>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<unsigned int> texturesIds, const char* _name)
{
    Vertices = vertices;
    Indices = indices;
    TexturesIds = texturesIds;

    vao = vbo = ebo = 0;

    numVertices = vertices.size();
    numIndices = indices.size();
    name = _name;
    model = float4x4::identity;

    setupMesh();
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    Vertices.clear();
    Vertices.shrink_to_fit();
    Indices.clear();
    Indices.shrink_to_fit();
    TexturesIds.clear();
    TexturesIds.shrink_to_fit();
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

void Mesh::Draw(const unsigned program, const float4x4& proj, const float4x4& view, const float4x4& _model)
{
    model = _model;
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
    glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

    glActiveTexture(GL_TEXTURE0);
    if (TexturesIds.size() > 0) {
        glBindTexture(GL_TEXTURE_2D, TexturesIds[0]);
        glUniform1i(glGetUniformLocation(program, "difuse"), 0);
    }    

    // draw mesh
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}