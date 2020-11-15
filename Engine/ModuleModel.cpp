#include "ModuleModel.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleTexture.h"
#include "Assimp/include/assimp/scene.h"
#include "Assimp/include/assimp/cimport.h"
#include "Assimp/include/assimp/postprocess.h"
#include "Mesh.h"
#include <vector>

using namespace std;

ModuleModel::ModuleModel() { }

ModuleModel::~ModuleModel() { }

void ModuleModel::Load(const char* file_name)
{
	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene)
	{
		// TODO: LoadTextures(scene->mMaterials, scene->mNumMaterials);
		// TODO: LoadMeshes(scene->mMeshes, scene->mNumMeshes);
        LoadMeshes(scene);
	}
	else
	{
		LOG("Error loading %s: %s", file_name, aiGetErrorString());
	}
}

void ModuleModel::LoadMeshes(const aiScene* scene)
{
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        meshesList.push_back(createMesh(mesh, scene));
    }
}

Mesh ModuleModel::createMesh(aiMesh* mesh, const aiScene* scene)
{
    //Vertices
    vector<Vertex> vertices;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        //positions
        float3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        //texture coordinates
        float2 vec;
        vec.x = mesh->mTextureCoords[0][i].x;
        vec.y = mesh->mTextureCoords[0][i].y;
        vertex.TexCoords = vec;

        vertices.push_back(vertex);
    }

    //Incices
    vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    //Textures 
    vector<unsigned int> texturesIds = loadMaterials(scene);

    //Mesh created from the extracted mesh data
    return Mesh(vertices, indices, texturesIds);
}

vector<unsigned int> ModuleModel::loadMaterials(const aiScene* scene)
{
    aiString file;
    vector<unsigned int> texturesIds;
    texturesList.reserve(scene->mNumMaterials);    
    for (unsigned i = 0; i < scene->mNumMaterials; ++i)
    {
        if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS)
        {
            texturesIds.push_back(App->texture->Load(file.data));
            texturesList.push_back(App->texture->Load(file.data));
        }
    }

    return texturesIds;
}

void ModuleModel::DrawMeshes(unsigned program, const float4x4& proj, const float4x4& view)
{
    for (unsigned int i = 0; i < meshesList.size(); i++)
    {
        meshesList[i].Draw(program, proj, view);
    }
}
