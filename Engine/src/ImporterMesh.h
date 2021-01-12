#pragma once
#include "Mesh.h"
#include "Libraries/Assimp/include/assimp/scene.h"


class ImporterMesh
{
public:
	void Import(const aiMesh* mesh, Mesh* ourMesh);
	unsigned int Save(const Mesh* ourMesh, char** fileBuffer);
	void Load(const char* fileBuffer, Mesh* ourMesh);

};

