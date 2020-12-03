#pragma once
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"
#include "Assimp/include/assimp/scene.h"
#include "Assimp/include/assimp/mesh.h"
#include "Mesh.h"
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

	const std::vector<Mesh*> GetMeshes() { return meshesList; }
	const std::vector<unsigned int> GetTexturesIds() { return texturesList; };
	void SetTexture(unsigned int textureId);

private:
	std::vector<unsigned int> texturesList;
	std::vector<Mesh*> meshesList;
	std::vector<std::string> pathList;
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
	Mesh* CreateMesh(const aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);
	void CalculateVolumeCenter();
};

