#include "Globals.h"
#include "ImporterMaterial.h"
#include "Libraries/Assimp/include/assimp/cimport.h"
#include "Application.h"
#include "ModuleTexture.h"
#include "Libraries/DevIL-SDK/include/IL/ilu.h"
#include "Libraries/DevIL-SDK/include/IL/ilut.h"

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

            bool skip = false;
            for (unsigned int j = 0; j < pathList.size(); j++)
            {
                if (pathList[j] == path || pathList[j] == (directory + file.data).c_str() || pathList[j] == (directoryTexture + file.data).c_str())
                {
                    ourMaterial->AddTexturePath(texturesList[j], pathList[j], typeIdList[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {    // if texture hasn't been loaded already, load it
                unsigned int texture = App->texture->Load(file.data);
                if (texture == -1)
                {
                    LOG("Texture is NOT on the path described in the FBX");
                    path = (directory + file.data).c_str();
                    texture = App->texture->Load(path.c_str());
                    if (texture == -1)
                    {
                        LOG("Texture is NOT on the same folder you loaded the FBX");
                        path = (directoryTexture + file.data).c_str();
                        texture = App->texture->Load(path.c_str());
                        if (texture == -1)
                        {
                            LOG("Texture is NOT on our own Textures/ folder");
                        }
                        else LOG("Texture loaded with our own Textures/ folder");
                    }
                    else LOG("Texture loaded with the same folder you loaded the FBX");
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
    unsigned int numTextures = ourMaterial->GetNunTextures();

    unsigned int sizeBody = 0;
    for (unsigned int i = 0; i < numTextures; ++i)
    {
        sizeBody += sizeof(unsigned int);
        sizeBody += 1 + ourMaterial->GetPaths()[i].length() * sizeof(char);
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

        //string s = ourMaterial->GetPaths()[i];
        //const char* path = s.c_str();
        bytes = sizeof(const char) * (sizePath +1);
        memcpy(cursor, ourMaterial->GetPaths()[i].c_str(), bytes);
        cursor += bytes;

        unsigned int typeId = ourMaterial->GetTypesId()[i];
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

        char* path = new char[sizePath];
        bytes = sizeof(char) * (sizePath +1);
        memcpy(path, cursor, bytes);
        cursor += bytes;

        unsigned int typeId;
        bytes = sizeof(unsigned int);
        memcpy(&typeId, cursor, bytes);
        cursor += bytes;

        string s = "Load sizePath: " + std::to_string(sizePath) + " path: " + path + " typeID: " + std::to_string(typeId);
        LOG(s.c_str());

        unsigned int texture = App->texture->Load(path);

        ourMaterial->AddTexturePath(texture, path, typeId);
    }
}


void ImporterMaterial::InitTexture()
{
    ilInit();
    iluInit();
    ilutInit();
    ilutRenderer(ILUT_OPENGL);
}

bool ImporterMaterial::ImportTexture(const char* buffer, unsigned size)
{
    if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size))
    {
        return true;
    }
    return false;
}

unsigned ImporterMaterial::SaveTexture(char** buffer)
{
    ilEnable(IL_FILE_OVERWRITE);

    ILuint size;
    ILubyte* saveBuffer;

    ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
    size = ilSaveL(IL_DDS, nullptr, 0);

    if (size > 0)
    {
        saveBuffer = new ILubyte[size];
        if (ilSaveL(IL_DDS, saveBuffer, size) > 0)
        {
            *buffer = (char*)saveBuffer;
        }
    }
    return size;
}

void ImporterMaterial::LoadTexture(const char* buffer, unsigned size, unsigned id)
{
    ILuint ImageName;
    ilGenImages(1, &ImageName);
    ilBindImage(ImageName);

    ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size);
    id = ilutGLBindTexImage();

    ilDeleteImages(1, &ImageName);
}