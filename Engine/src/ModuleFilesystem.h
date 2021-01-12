#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include <string>

class ModuleFilesystem : public Module
{
public:
	ModuleFilesystem();

	bool Init() override;

	unsigned int Save() const;
	unsigned int Load() const;
	unsigned int Save(const char* file, const char* buffer, unsigned int size, bool append = false) const;
	unsigned int Load(const char* path, const char* file, char** buffer) const;
	/*bool Remove(const char* file);
	bool Exists(const char* file) const;
	bool MakeDirectory(const char* directory);
	bool IsDirectory(const char* file) const;
	bool Copy(const char* source, const char* destination);*/

private:
	void CreateLibraryDirectories();
	bool MakeDirectory(const char* dir);
	void GetAllFilesWithExtension(const char* myPath, const char* extension, std::vector<std::string>& fileList, std::vector<std::string>& dirList);

	std::string basePath;

};


