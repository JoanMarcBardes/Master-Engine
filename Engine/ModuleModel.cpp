#include "ModuleModel.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleTexture.h"
#include "Assimp/include/assimp/scene.h"
#include "Assimp/include/assimp/cimport.h"
#include "Assimp/include/assimp/postprocess.h"
#include "Mesh.h"
#include "DebugLeaks.h"
#include <vector>
#include <string>

using namespace std;

void PrintLogAssimp(const char* message, char* user) { LOG(message); }

ModuleModel::ModuleModel() 
{
}

ModuleModel::~ModuleModel() 
{    
}

bool ModuleModel::CleanUp() 
{
    texturesList.clear();
    texturesList.shrink_to_fit();
    meshesList.clear();
    meshesList.shrink_to_fit();
    aiDetachAllLogStreams();

    return true;
}

void ModuleModel::Load(const char* file_name)
{
    LOG("ModuleModel Load\n");
    struct aiLogStream stream;
    stream.callback = PrintLogAssimp;
    aiAttachLogStream(&stream);
    
	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene)
	{
		// TODO: LoadTextures(scene->mMaterials, scene->mNumMaterials);
		// TODO: LoadMeshes(scene->mMeshes, scene->mNumMeshes);
        string path = string(file_name);
        directory = path.substr(0, path.find_last_of('/'));
        directory += "/";
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

Mesh ModuleModel::createMesh(const aiMesh* mesh, const aiScene* scene)
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
    //vector<unsigned int> texturesIds = loadMaterials(scene);
    vector<unsigned int> texturesIds = loadMaterials2(mesh,scene);

    //Mesh created from the extracted mesh data
    return Mesh(vertices, indices, texturesIds, mesh->mName.C_Str());
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

vector<unsigned int> ModuleModel::loadMaterials2(const aiMesh* mesh, const aiScene* scene)
{
    aiString file;
    vector<unsigned int> texturesIds;
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    unsigned int nDIFFUSE = material->GetTextureCount(aiTextureType_DIFFUSE);
    unsigned int nSPECULAR = material->GetTextureCount(aiTextureType_SPECULAR);
    unsigned int nHEIGHT = material->GetTextureCount(aiTextureType_HEIGHT);
    unsigned int nAMBIENT = material->GetTextureCount(aiTextureType_AMBIENT);

    for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
    {
        material->GetTexture(aiTextureType_DIFFUSE, i, &file);
        string path = (directory + file.data).c_str();

        bool skip = false;
        for (unsigned int j = 0; j < pathList.size(); j++)
        {
            if (pathList[j] == path)
            {
                texturesIds.push_back(texturesList[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            texturesIds.push_back(App->texture->Load(path.c_str()));
            texturesList.push_back(App->texture->Load(path.c_str()));
            pathList.push_back(path);
        }
        path.clear();
    }

    return texturesIds;
}

void ModuleModel::DrawMeshes(const unsigned program, const float4x4& proj, const float4x4& view, const float4x4& model)
{
    for (unsigned int i = 0; i < meshesList.size(); i++)
    {
        meshesList[i].Draw(program, proj, view, model);
    }
}
