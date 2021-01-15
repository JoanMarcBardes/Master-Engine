#pragma once
#include "Mesh.h"
#include "Libraries/Assimp/include/assimp/scene.h"


class ImporterMesh
{
public:
	Mesh* Import(const aiMesh* mesh);
	unsigned int Save(const Mesh* ourMesh, char** fileBuffer);
	void Load(const char* fileBuffer, Mesh* ourMesh);

};

