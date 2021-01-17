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
    textureTypesList.push_back(aiTextureType_DIFFUSE);
    textureTypesList.push_back(aiTextureType_SPECULAR);

    importMesh = new ImporterMesh();
    importMat = new ImporterMaterial();
}

ModuleModel::~ModuleModel() 
{    
}

bool ModuleModel::CleanUp() 
{
    /*for (unsigned i = 0; i < texturesList.size(); ++i) {
        App->texture->DeleteTexture(texturesList[i]);
    }

    for (std::vector<Mesh*>::iterator it = meshesList.begin(); it != meshesList.end(); ++it)
    {
        delete* it;
    }*/

    texturesList.clear();
    meshesList.clear();
    pathList.clear();   
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
        string path = string(file_name);
        directory = path.substr(0, path.find_last_of('/'));
        directory += "/";

        string name = path.substr(directory.size());
        //LoadMeshes(scene);
        //LoadMaterials(scene);

        GameObject* parent = App->scene->CreateGameObject(name.c_str());
        processNode(scene->mRootNode, scene, parent);

        CalculateVolumeCenter();
	}
	else
	{
		LOG("[error] Error loading %s: %s", file_name, aiGetErrorString());
	}
}

void ModuleModel::LoadMeshes(const aiScene* scene)
{
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        meshesList.push_back(CreateMesh(mesh, scene));
    }
}

void ModuleModel::processNode(aiNode* node, const aiScene* scene, GameObject* parent)
{
    if(node->mNumMeshes > 1)
        parent = App->scene->CreateGameObject(node->mName.C_Str(), parent);

    GameObject* child = nullptr;
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        GameObject* child = App->scene->CreateGameObject(mesh->mName.C_Str(), parent);

        Mesh* newMesh = CreateMesh(mesh, scene);
        child->AddComponent(newMesh);
        meshesList.push_back(newMesh);

        Material* material = LoadMaterials(scene->mMaterials[mesh->mMaterialIndex]);
        child->AddComponent(material);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, parent);
    }

}

Mesh* ModuleModel::CreateMesh(const aiMesh* mesh, const aiScene* scene)
{
    //Vertices
    vector<Mesh::Vertex> vertices;
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
    vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }    

    //Mesh created from the extracted mesh data
    return new Mesh(vertices, indices, mesh->mName.C_Str());
}

Material* ModuleModel::LoadMaterials(const aiMaterial* material)
{
    aiString file;
    string path;
    string pathSameFolder;
    string pathTexture;

    std::vector<unsigned int> newTextures;
    std::vector<std::string> newPath;
    std::vector<unsigned int> newTypeId;

    aiMaterialProperty** matPro = material->mProperties;

    for each (aiTextureType type in textureTypesList)
    {        
        for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
        {
            material->GetTexture(type, i, &file);

            path = file.data;
            pathSameFolder = (directory + file.data).c_str();
            pathTexture = (directoryTexture + file.data).c_str();

            bool skip = false;
            for (unsigned int j = 0; j < pathList.size(); j++)
            {
                if (pathList[j] == path || pathList[j] == pathSameFolder || pathList[j] == pathTexture)
                {
                    newTextures.push_back(texturesList[j]);
                    newPath.push_back(pathList[j]);
                    newTypeId.push_back(typeIdList[j]);

                    skip = true;
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                GLuint texture = App->texture->Load(file.data);
                if (texture == -1)
                {
                    LOG("Texture is NOT on the path described in the FBX");
                    texture = App->texture->Load(pathSameFolder.c_str());
                    if (texture == -1)
                    {
                        LOG("Texture is NOT on the same folder you loaded the FBX");
                        texture = App->texture->Load(pathTexture.c_str());
                        if (texture == -1)
                        {
                            LOG("Texture is NOT on our own Textures/ folder");
                        }
                        else LOG("Texture loaded with our own Textures/ folder");
                    }
                    else LOG("Texture loaded with the same folder you loaded the FBX");
                }
                else LOG("Texture loaded with the path described in the FBX");

                newTextures.push_back(texture);
                texturesList.push_back(texture);
                newPath.push_back(path);
                pathList.push_back(path);
                newTypeId.push_back(GetTypeId(type));
                typeIdList.push_back(GetTypeId(type));
            }
            path.clear();
        }
    }
    

    if (newTextures.size() == 0)
    {
        LOG("[error] No texture");
    }

    return new Material(newTextures, newPath, newTypeId);
}

void ModuleModel::DrawMeshes(const unsigned program)
{
    for (std::vector<Mesh*>::iterator it = meshesList.begin(); it != meshesList.end(); ++it)
    {
        glActiveTexture(GL_TEXTURE0);
        //for (unsigned int i = 0; i < texturesList.size(); ++i) {
        glBindTexture(GL_TEXTURE_2D, texturesList[0]);
        glUniform1i(glGetUniformLocation(program, "my_texture"), 0);
        //}

        (*it)->Draw();
    }
}


/*void ModuleModel::SetTexture(unsigned int textureId)
{
    for (unsigned i = 0; i < texturesList.size(); ++i) {
        App->texture->DeleteTexture(texturesList[i]);
    }
    texturesList.clear();
    texturesList.push_back(textureId);
}*/

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

void ModuleModel::CalculateVolumeCenter()
{
    min = (*meshesList.begin())->GetMin();
    max = (*meshesList.begin())->GetMax();
    float3 Min;
    float3 Max;

    for (std::vector<Mesh*>::iterator it = meshesList.begin(); it != meshesList.end(); ++it)
    {
        Min =(*it)->GetMin();
        Max =(*it)->GetMax();
        //min
        if (Min.x < min.x)
            min.x = Min.x;
        if (Min.y < min.y)
            min.y = Min.y;
        if (Min.z < min.z)
            min.z = Min.z;

        //max
        if (Max.x > max.x)
            max.x = Max.x;
        if (Max.y > max.y)
            max.y = Max.y;
        if (Max.z > max.z)
            max.z = Max.z;
    }

    width = max.x - min.x;
    height = max.y - min.y;
    lenght = max.z - min.z;

    volume = width * height;// *lenght;
    center = float3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);

    //App->editorCamera->SetTarget(center);
    //App->editorCamera->AdaptSizeGeometry(volume);
}

unsigned int ModuleModel::GetTypeId(aiTextureType textureType)
{
    unsigned int typeId = -1;
    switch (textureType)
    {
    case aiTextureType_DIFFUSE:
        typeId = 0;
        break;
    case aiTextureType_SPECULAR:
        typeId = 1;
        break;
    }
    return typeId;
}


//

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

void ModuleModel::Load(const char* dir, const char* name, unsigned int type)
{
    char* buffer;
    App->filesystem->Load(dir, ""/*name*/, &buffer);
    LOG((string("LOAD ") + name).c_str());

    if (type == 0)
    {
        Mesh* mesh = new Mesh();
        ImporterMesh* importMesh = new ImporterMesh();
        importMesh->Load(buffer, mesh);
        LOG("LOAD mesh");
    }
    else
    {
        Material * material = new Material();
        ImporterMaterial* importMat = new ImporterMaterial();
        importMat->Load(buffer, material);

        LOG("LOAD material");
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