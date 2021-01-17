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

	void InitTexture();
	bool ImportTexture(const char* buffer, unsigned size);
	unsigned SaveTexture(char** buffer);
	void LoadTexture(const char* buffer, unsigned size, unsigned id);

	void SetDirectory(std::string dir) { directory = dir; }

private:
	std::vector<aiTextureType> textureTypesList;

	std::vector<unsigned int> texturesList;
	std::vector<std::string> pathList;
	std::vector<unsigned int> typeIdList;

	std::string directory;
	std::string directoryTexture = "Textures/";
};

