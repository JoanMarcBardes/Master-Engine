#include "ImporterMesh.h"
#include <vector>
#include <string>

Mesh* ImporterMesh::Import(const aiMesh* mesh)
{
    //Vertices
    std::vector<Mesh::Vertex> vertices;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Mesh::Vertex vertex;

        //positions
        float3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        // normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        //texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            float2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = float2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    //Incices
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return new Mesh(vertices, indices, mesh->mName.C_Str());
}

unsigned int ImporterMesh::Save(const Mesh* ourMesh, char** fileBuffer)
{
    // numVertices, numIndices, vertices, indices
    unsigned int size = sizeof(unsigned int) + sizeof(unsigned int) +
        sizeof(Mesh::Vertex) * ourMesh->GetNumVertices() + 
        sizeof(unsigned int) * ourMesh->GetNumIndices();
      

    *fileBuffer = new char[size];
    char* cursor = *fileBuffer;

    unsigned int numVertices = ourMesh->GetNumVertices();
    unsigned int bytes = sizeof(unsigned int);
    memcpy(cursor, &numVertices, bytes);
    cursor += bytes;

    unsigned int numIndices = ourMesh->GetNumIndices();
    bytes = sizeof(unsigned int);
    memcpy(cursor, &numIndices, bytes);
    cursor += bytes;

    unsigned a = sizeof(Mesh::Vertex);
    unsigned b = a * numVertices;

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        Mesh::Vertex vertex = ourMesh->GetVertices()[i];
        bytes = sizeof(Mesh::Vertex);
        memcpy(cursor, &vertex, bytes);
        cursor += bytes;
    }

    for (unsigned int i = 0; i < numIndices; ++i)
    {
        unsigned int indices = ourMesh->GetIndices()[i];
        bytes = sizeof(unsigned int);
        memcpy(cursor, &indices, bytes);
        cursor += bytes;
    }

    return size;
}

void ImporterMesh::Load(const char* fileBuffer, Mesh* &ourMesh)
{
    const char* cursor = fileBuffer;

    unsigned int numVertices = 0;
    unsigned int bytes = sizeof(unsigned int);
    memcpy(&numVertices, cursor, bytes);
    cursor += bytes;

    unsigned int numIndices = 0;
    bytes = sizeof(unsigned int);
    memcpy(&numIndices, cursor, bytes);
    cursor += bytes;

    std::vector<Mesh::Vertex> vertices;
    for (unsigned int i = 0; i < numVertices; ++i)
    {
        Mesh::Vertex vertex;
        bytes = sizeof(Mesh::Vertex);
        memcpy(&vertex, cursor, bytes);
        cursor += bytes;
        vertices.push_back(vertex);
    }

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < numIndices; ++i)
    {
        unsigned int indice;
        bytes = sizeof(unsigned int);
        memcpy(&indice, cursor, bytes);
        cursor += bytes;
        indices.push_back(indice);
    }

    ourMesh = new Mesh(vertices, indices, "mesh");
}