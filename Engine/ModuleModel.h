#pragma once
#include "Module.h"
#include "Globals.h"
#include "Assimp/include/assimp/scene.h"
#include "GL/glew.h"
#include "Assimp/include/assimp/mesh.h"
#include "Mesh.h"
#include <vector>

class ModuleModel : public Module
{
public:
	ModuleModel();
	~ModuleModel();

	void Load(const char* file_name);
	void LoadMeshes(const aiScene* scene);

	Mesh createMesh(const aiMesh* mesh, const aiScene* scene);
	std::vector<unsigned int> loadMaterials(const aiScene* scene);
	void DrawMeshes(const unsigned program, const float4x4& proj, const float4x4& view);

private:
	std::vector<unsigned int> texturesList;
	std::vector<Mesh> meshesList;
};
