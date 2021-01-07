#pragma once
#include "Material.h"
#include "Libraries/Assimp/include/assimp/scene.h"

class ImporterMaterial
{
public:
	ImporterMaterial();

	void Import(const aiMaterial* material, Material* ourMaterial);
	unsigned int Save(const Material* ourMaterial, char** fileBuffer);
	void Load(const char* fileBuffer, Material* ourMaterial);

private:
	std::vector<aiTextureType> textureTypesList;
};

