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
    GetAllFilesWithExtension(Concat(basePath, "Models\\"), ".fbx", fileList, dirList);

    return true;
}

void ModuleFilesystem::CreateLibraryDirectories()
{
	CreateDir("Library/");
	CreateDir("Library/Meshes/");
	CreateDir("Library/Materials/");
}

bool ModuleFilesystem::CreateDir(const char* dir)
{
	int stat = _mkdir( Concat(basePath, dir) );
    return !stat;
}

char* ModuleFilesystem::Concat(const char* a, const char* b)
{
    // Determine new size
    int newSize = strlen(a) + strlen(b) + 1;

    // Allocate new buffer
    char* newBuffer = (char*)malloc(newSize);

    // do the copy and concat
    strcpy(newBuffer, a);
    strcat(newBuffer, b); // or strncat

    return newBuffer;
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