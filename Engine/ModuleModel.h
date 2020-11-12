#pragma once
#include "Module.h"
#include "Globals.h"
#include "Assimp/include/assimp/scene.h"

class ModuleModel : public Module
{
public:
	ModuleModel();
	~ModuleModel();

	void Load(const char* file_name);
	void LoadMaterials(const aiScene* scene);

private:
};

