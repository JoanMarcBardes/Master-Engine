#include "Globals.h"
#include "ImporterMaterial.h"
#include "Libraries/Assimp/include/assimp/cimport.h"
#include "Application.h"
#include "ModuleTexture.h"

#include <vector>
#include <string>

using namespace std;

unsigned int GetTypeId(aiTextureType textureType)
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

    unsigned int sizeBody = 0;
    for (unsigned int i = 0; i < numTextures; ++i)
    {
        sizeBody += sizeof(unsigned int);
        sizeBody += ourMaterial->GetPaths()[i].length() * sizeof(char);
        sizeBody += sizeof(unsigned int);
    }
    // Head + body
    unsigned int size = sizeof(unsigned int) + sizeBody;

    *fileBuffer = new char[size];
    char* cursor = *fileBuffer;

    //Head
    unsigned int bytes = sizeof(unsigned int);
    memcpy(cursor, &numTextures, bytes);
    cursor += bytes;

    //Body
    for (unsigned int i = 0; i < numTextures; ++i)
    {
        unsigned int sizePath = ourMaterial->GetPaths()[i].length();
        bytes = sizeof(unsigned int);
        memcpy(cursor, &sizePath, bytes);
        cursor += bytes;

        string s = ourMaterial->GetPaths()[i];
        const char* path = s.c_str();
        bytes = sizeof(const char) * sizePath;
        memcpy(cursor, &ourMaterial->GetPaths()[i], bytes);
        cursor += bytes;

        unsigned int typeId = 12;// ourMaterial->GetTypeId()[i];
        bytes = sizeof(unsigned int);
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
        unsigned int sizePath;
        bytes = sizeof(unsigned int);
        memcpy(&sizePath, cursor, bytes);
        cursor += bytes;

        char* path;
        bytes = sizeof(char) * sizePath;
        memcpy(&path, cursor, bytes);
        cursor += bytes;

        unsigned int typeId;
        bytes = sizeof(unsigned int);
        memcpy(&typeId, cursor, bytes);
        cursor += bytes;

        string s = "Load sizePath: " + std::to_string(sizePath) + " path: " + path + " typeID: " + std::to_string(typeId);
        LOG(s.c_str());

        unsigned int texture = 0;// App->texture->Load(path);

        ourMaterial->AddTexturePath(texture, path, typeId);
    }
}