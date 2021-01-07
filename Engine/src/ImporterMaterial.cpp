#include "Globals.h"
#include "ImporterMaterial.h"
#include "Libraries/Assimp/include/assimp/cimport.h"
#include "Application.h"
#include "ModuleTexture.h"

#include <vector>
#include <string>

using namespace std;

std::string GetTypeId(aiTextureType textureType)
{
    std::string typeId = "";
    switch (textureType)
    {
    case aiTextureType_DIFFUSE:
        typeId = "diffuse_map";
        break;
    case aiTextureType_SPECULAR:
        typeId = "specular_map";
        break;
    default:
        typeId = "NullType";
        break;
    }
    return typeId;
}

ImporterMaterial::ImporterMaterial()
{
    textureTypesList.push_back(aiTextureType_DIFFUSE);
    textureTypesList.push_back(aiTextureType_SPECULAR);
}

void ImporterMaterial::Import(const aiMaterial* material, Material* ourMaterial)
{
    aiString file;
    string path;

    aiMaterialProperty** matPro = material->mProperties;

    for each (aiTextureType type in textureTypesList)
    {
        for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
        {
            material->GetTexture(type, i, &file);

            path = file.data;

            {   // if texture hasn't been loaded already, load it
                unsigned int texture = App->texture->Load(file.data);
                if (texture == -1)
                {
                    LOG("Texture is NOT on the path described in the FBX");
                }
                else LOG("Texture loaded with the path described in the FBX");

                ourMaterial->AddTexturePath(texture, path, GetTypeId(type));
            }
            path.clear();
        }
    }

    if (ourMaterial->GetTextures().size() == 0)
    {
        LOG("[error] No texture");
    }
}

unsigned int ImporterMaterial::Save(const Material* ourMaterial, char** fileBuffer)
{
    const unsigned int numTextures = ourMaterial->GetNunTextures();
    //numTextures + textures, paths, typeId * numTextures
    unsigned int size = sizeof(unsigned int) + (sizeof(unsigned int) + sizeof(std::string) + sizeof(std::string)) * numTextures;

    *fileBuffer = new char[size];
    char* cursor = *fileBuffer;

    for (unsigned int i = 0; i < numTextures; ++i)
    {
        unsigned int texture = ourMaterial->GetTextures()[i];
        unsigned int bytes = sizeof(unsigned int);
        memcpy(cursor, &texture, bytes);
        cursor += bytes;

        std::string path = ourMaterial->GetPaths()[i];
        bytes = sizeof(std::string);
        memcpy(cursor, &path, bytes);
        cursor += bytes;

        std::string typeId = ourMaterial->GetTypeId()[i];
        bytes = sizeof(std::string);
        memcpy(cursor, &typeId, bytes);
        cursor += bytes;
    }

    return size;
}

void ImporterMaterial::Load(const char* fileBuffer, Material* ourMaterial)
{
    const char* cursor = fileBuffer;

    unsigned int numTextures = 0;
    unsigned int bytes = sizeof(unsigned int);
    memcpy(&numTextures, cursor, bytes);
    cursor += bytes;

    for (unsigned int i = 0; i < numTextures; ++i)
    {
        unsigned int texture = 0;
        unsigned int bytes = sizeof(unsigned int);
        memcpy(&texture, cursor, bytes);
        cursor += bytes;

        std::string path = 0;
        bytes = sizeof(std::string);
        memcpy(&path, cursor, bytes);
        cursor += bytes;

        std::string typeId = 0;
        bytes = sizeof(std::string);
        memcpy(&typeId, cursor, bytes);
        cursor += bytes;

        ourMaterial->AddTexturePath(texture, path, typeId);
    }
}