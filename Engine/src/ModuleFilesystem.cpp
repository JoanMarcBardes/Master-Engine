#include "ModuleFilesystem.h"
#include "SDL.h"

#include <direct.h>
#include <filesystem>

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
    GetAllFilesWithExtension((basePath + "Models\\").c_str(), ".fbx", fileList, dirList);

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