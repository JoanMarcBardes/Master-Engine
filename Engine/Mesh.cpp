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
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::Draw(const unsigned program, const float4x4& proj, const float4x4& view, const float4x4& _model, std::vector<unsigned int>& texturesList)
{
    model = _model;
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
    glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

    glActiveTexture(GL_TEXTURE0);
    //for (unsigned int i = 0; i < texturesList.size(); ++i) {
    glBindTexture(GL_TEXTURE_2D, texturesList[0]);
    glUniform1i(glGetUniformLocation(program, "my_texture"), 0);
    //}

    float3 camera_pos = float3(0,0,5);
    float3 light_dir = float3(1, 0, 0);
    float4 light_color = float4(1,1,1,1);
    float4 ambient_color = float4(0.1f, 0.1f, 0.1f, 1.0f);
    float Ks = 0.7f;
    float Kd = 0.6f;
    int shininess = 70;

    glUniform3f(glGetUniformLocation(program, "camera_pos"), camera_pos.x, camera_pos.y, camera_pos.z);
    glUniform3f(glGetUniformLocation(program, "light_dir"), light_dir.x, light_dir.y, light_dir.z);
    glUniform4f(glGetUniformLocation(program, "light_color"), light_color.x, light_color.y, light_color.z, light_color.w);
    glUniform4f(glGetUniformLocation(program, "ambient_color"), ambient_color.x, ambient_color.y, ambient_color.z, ambient_color.w);
    glUniform1f(glGetUniformLocation(program, "Ks"), Ks);
    glUniform1f(glGetUniformLocation(program, "Kd"), Kd);
    glUniform1i(glGetUniformLocation(program, "shininess"), shininess);

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
