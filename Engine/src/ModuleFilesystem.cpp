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
    //Save();
    //Load();

    /*std::vector<std::string> fileList;
    std::vector<std::string> dirList;
    GetAllFilesWithExtension((basePath + "Models\\").c_str(), ".fbx", fileList, dirList);
    for (unsigned i = 0; i < dirList.size(); ++i)
    {
        std::string name = fileList[i].substr(0, fileList[i].find_last_of('.'));
        App->model->Import(dirList[i].c_str());
    }

    fileList.clear();
    dirList.clear();
    GetAllFilesWithExtension((basePath + "Library\\Meshes").c_str(), ".meta", fileList, dirList);
    for (unsigned i = 0; i < dirList.size(); ++i)
    {
        App->model->Load(dirList[i].c_str(), fileList[i].c_str(), 0);
    }

    fileList.clear();
    dirList.clear();
    GetAllFilesWithExtension((basePath + "Library\\Materials").c_str(), ".meta", fileList, dirList);
    for (unsigned i = 0; i < dirList.size(); ++i)
    {
        App->model->Load(dirList[i].c_str(), fileList[i].c_str(), 1);
    }*/

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

std::string ModuleFilesystem::Save(const char* file, const char* buffer, unsigned int size, bool append) const
{
    std::ofstream fout;
    std::string path = basePath + std::string("Library\\") + file + ".meta";
    fout.open(path.c_str(), std::ios::binary | std::ios::out);

    fout.write(buffer, size);

    fout.close();

    return path;
}


unsigned int ModuleFilesystem::Load(const char* path, const char* file, char** buffer) const
{
    //std::ifstream infile((basePath + "Library\\" + file + ".meta").c_str(), std::ios::binary);
    std::string s = basePath + std::string(path) + file;
    std::ifstream infile(s.c_str(), std::ios::binary);

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