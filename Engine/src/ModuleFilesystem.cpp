#include "ModuleFilesystem.h"
#include "SDL.h"
#include "Application.h"
#include "ModuleModel.h"

#include <direct.h>
#include <filesystem>
#include <fstream>

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

ModuleFilesystem::ModuleFilesystem()
{
	basePath = SDL_GetBasePath();

	CreateLibraryDirectories();
}

bool ModuleFilesystem::Init()
{
    std::vector<std::string> fileList;
    std::vector<std::string> dirList;
    GetAllFilesWithExtension((basePath + "Assets\\").c_str(), ".fbx", fileList, dirList);
    for (unsigned i = 0; i < dirList.size(); ++i)
    {
        std::string name = fileList[i].substr(0, fileList[i].find_last_of('.'));
        App->model->Import(dirList[i].c_str(), name.c_str());
    }

    fileList.clear();
    dirList.clear();
    GetAllFilesWithExtension((basePath + "Library\\").c_str(), ".meta", fileList, dirList);
    for (unsigned i = 0; i < dirList.size(); ++i)
    {
        App->model->Load(dirList[i].c_str(), fileList[i].c_str(), i);
    }

    //Save();
    //Load();

    return true;
}

void ModuleFilesystem::CreateLibraryDirectories()
{
	MakeDirectory("Library/");
	MakeDirectory("Library/Meshes/");
	MakeDirectory("Library/Materials/");
}

bool ModuleFilesystem::MakeDirectory(const char* dir)
{
	int stat = _mkdir( (basePath + dir).c_str() );
    return !stat;
}

void ModuleFilesystem::GetAllFilesWithExtension(const char* myPath, const char* extension, std::vector<std::string>& fileList, std::vector<std::string>& dirList)
{
    for (const auto& dirEntry : recursive_directory_iterator(myPath))
    {
        if (dirEntry.path().extension().string() == extension)
        {
            fileList.push_back(dirEntry.path().filename().string());
            dirList.push_back(dirEntry.path().string());
        }

        LOG(dirEntry.path().string().c_str());
    }
}

unsigned int ModuleFilesystem::Save() const
{
    char* aux = "Hello Word";
    unsigned int i = strlen(aux) * sizeof(char);
    unsigned int size = sizeof(unsigned int) + i;

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(unsigned int);
    memcpy(cursor, &i, bytes);
    cursor += bytes;

    bytes = i;
    memcpy(cursor, &aux, bytes);
    cursor += bytes;

    //
    std::ofstream fout;
    fout.open((basePath + "Library\\file.bin").c_str(), std::ios::binary | std::ios::out);

    fout.write(fileBuffer, size);

    fout.close();

    LOG("SAVE: %u", size);

    return 0;
}


unsigned int ModuleFilesystem::Load() const
{
    std::ifstream infile((basePath + "Library\\file.bin").c_str(), std::ios::binary);
    char* buffer;

    //get length of file
    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);
    buffer = new char[length];

    //read file
    if (length > 0) {
        infile.read(buffer, length);
    }
    else
    {
        LOG("[error] ModuleFilesystem::Load");
    }

    infile.close();

    //
    const char* cursor = buffer;

    unsigned int num;
    unsigned int bytes = sizeof(unsigned int);
    memcpy(&num, cursor, bytes);
    cursor += bytes;

    char* text;
    bytes = sizeof(char) * num;
    memcpy(&text, cursor, bytes);
    cursor += bytes;

    LOG(text);

    return 0;
}

unsigned int ModuleFilesystem::Save(const char* file, const char* buffer, unsigned int size, bool append) const
{
    std::ofstream fout;
    fout.open((basePath + "Library\\" + file + ".meta").c_str(), std::ios::binary | std::ios::out);

    fout.write(buffer, size);

    fout.close();

    LOG("SAVE: %u", size);

    return 0;
}


unsigned int ModuleFilesystem::Load(const char* path, const char* file, char** buffer) const
{
    //std::ifstream infile((basePath + "Library\\" + file + ".meta").c_str(), std::ios::binary);
    std::ifstream infile(path, std::ios::binary);

    //get length of file
    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);
    *buffer = new char[length];

    //read file
    if (length > 0) {
        infile.read(*buffer, length);
    }
    else
    {
        LOG("[error] ModuleFilesystem::Load");
    }

    infile.close();

    return 0;
}