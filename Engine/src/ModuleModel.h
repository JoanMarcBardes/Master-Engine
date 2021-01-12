#pragma once
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"
#include "Libraries/Assimp/include/assimp/scene.h"
#include "Libraries/Assimp/include/assimp/mesh.h"
#include "Mesh.h"
#include "Material.h"
#include <vector>
#include <string>

class ModuleModel : public Module
{
public:
	ModuleModel();
	~ModuleModel();
	bool CleanUp();

	void Load(const char* file_name);	
	void DrawMeshes(const unsigned program);

	void Import(const char* dir, const char* name);
	void ImportNode(aiNode* node, const aiScene* scene, std::string name);
	void Load(const char* dir, const char* name, unsigned int type);


	const std::vector<Mesh*> GetMeshes() { return meshesList; }
	const std::vector<unsigned int> GetTexturesIds() { return texturesList; };
	void SetTexture(unsigned int textureId);

private:
	std::vector<aiTextureType> textureTypesList;
	std::vector<unsigned int> texturesList;
	std::vector<Mesh*> meshesList;
	std::vector<std::string> pathList;
	std::vector<unsigned int> typeIdList;
	std::string directory;
	std::string directoryTexture = "Textures/";
	float3 min;
	float3 max;
	float3 center;
	float width = 0;
	float height = 0;
	float lenght = 0;
	float volume = 0;

	void LoadMeshes(const aiScene* scene);
	void processNode(aiNode* node, const aiScene* scene, GameObject* parent = nullptr);

	Mesh* CreateMesh(const aiMesh* mesh, const aiScene* scene);
	Material* LoadMaterials(const aiMaterial* material);

	void CalculateVolumeCenter();
	unsigned int GetTypeId(aiTextureType textureType);
};

