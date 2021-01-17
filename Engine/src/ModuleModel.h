#pragma once
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"
#include "Libraries/Assimp/include/assimp/scene.h"
#include "Libraries/Assimp/include/assimp/mesh.h"
#include "Mesh.h"
#include "Material.h"
#include "ImporterMaterial.h"
#include "ImporterMesh.h"
#include <vector>
#include <string>

class ModuleModel : public Module
{
public:
	ImporterMesh* importMesh;
	ImporterMaterial* importMat;

	ModuleModel();
	~ModuleModel();
	bool CleanUp();

	void Import(const char* dir);
	void ImportNode(aiNode* node, const aiScene* scene, std::string name, GameObject* parent = nullptr);
	Mesh* LoadMesh(const char* path);
	Material* LoadMaterial(const char* path);

	void SetTexture(unsigned int textureId, std::string path, unsigned int newtypeId = 0);

private:
	std::string directory;
};

