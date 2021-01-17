#include "ModuleModel.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleTexture.h"
#include "ModuleEditorCamera.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "Libraries/Assimp/include/assimp/cimport.h"
#include "Libraries/Assimp/include/assimp/postprocess.h"
#include "Mesh.h"
#include "DebugLeaks.h"

#include "ModuleFilesystem.h"

#include <vector>
#include <string>

using namespace std;

void PrintLogAssimp(const char* message, char* user) { LOG(message); }

ModuleModel::ModuleModel() 
{
    importMesh = new ImporterMesh();
    importMat = new ImporterMaterial();

    LOG("ModuleModel\n");
    struct aiLogStream stream;
    stream.callback = PrintLogAssimp;
    aiAttachLogStream(&stream);
}

ModuleModel::~ModuleModel() 
{    
}

bool ModuleModel::CleanUp() 
{
    delete(importMesh);
    delete(importMat);
    aiDetachAllLogStreams();

    return true;
}

void ModuleModel::Import(const char* dir)
{
    const aiScene* scene = aiImportFile(dir, aiProcessPreset_TargetRealtime_MaxQuality);
    if (scene)
    {
        string path = string(dir);
        directory = path.substr(0, path.find_last_of('/'));
        directory += "/";
        string name = path.substr(directory.size());

        GameObject* parent = App->scene->CreateGameObject(name.c_str());
        ImportNode(scene->mRootNode, scene, name, parent);
    }
    else
    {
        LOG("[error] Error loading %s: %s", dir, aiGetErrorString());
    }
}

void ModuleModel::ImportNode(aiNode* node, const aiScene* scene, string name, GameObject* parent)
{
    if (node->mNumMeshes > 1)
        parent = App->scene->CreateGameObject(node->mName.C_Str(), parent);

    GameObject* child = nullptr;
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        GameObject* child = App->scene->CreateGameObject(mesh->mName.C_Str(), parent);
                
        Mesh* newMesh = importMesh->Import(mesh);
        child->AddComponent(newMesh);

        char* buffer;
        unsigned int size = importMesh->Save(newMesh, &buffer);
        std::string customPath = App->filesystem->Save(("Meshes\\" + name + std::to_string(i)).c_str(), buffer, size);
        customPath.erase(0, App->filesystem->basePath.size());
        newMesh->SetCustomPath(customPath);


        Material* material = new Material();        
        importMat->SetDirectory(directory);
        importMat->Import(scene->mMaterials[mesh->mMaterialIndex], material);
        child->AddComponent(material);

        size = importMat->Save(material, &buffer);
        customPath = App->filesystem->Save(("Materials\\" + name + std::to_string(i)).c_str(), buffer, size);
        customPath.erase(0, App->filesystem->basePath.size());
        material->SetCustomPath(customPath);

    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ImportNode(node->mChildren[i], scene, name + "_child" + std::to_string(i), parent);
    }

}

Mesh* ModuleModel::LoadMesh(const char* path)
{
    char* buffer;
    App->filesystem->Load(path, "", &buffer);
    
    Mesh* mesh = new Mesh();
    ImporterMesh* importMesh = new ImporterMesh();
    importMesh->Load(buffer, mesh);
    LOG("LOAD mesh");
    
    return mesh;
}

Material* ModuleModel::LoadMaterial(const char* path)
{
    char* buffer;
    App->filesystem->Load(path, "", &buffer);
    
    Material* material = new Material();
    ImporterMaterial* importMat = new ImporterMaterial();
    importMat->Load(buffer, material);
    LOG("LOAD material");

    return material;
}

void ModuleModel::SetTexture(unsigned int textureId, std::string path, unsigned int newtypeId)
{
    GameObject* go = App->editor->GetSelectedGameObject();
    if (go != nullptr)
    {
        Material* material2 = go->GetComponent< Material>();
        LOG(go->name.c_str());
        path.erase(0, App->filesystem->basePath.size());
        unsigned toRemove = go->SetTexture(textureId, path, newtypeId);
        App->texture->DeleteTexture(toRemove);

        char* buffer;
        Material* material = go->GetComponent< Material>();
        unsigned size = importMat->Save(material, &buffer);
        std::string customPath = App->filesystem->Save(("Materials\\" + go->name).c_str(), buffer, size);
        customPath.erase(0, App->filesystem->basePath.size());
        material->SetCustomPath(customPath);
    }
    else
    {
        LOG("nullptr go");
    }
}