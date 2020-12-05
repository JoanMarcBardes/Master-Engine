#pragma once
#include "Module.h"
#include "Globals.h"
#include "Libraries/DevIL-SDK/include/IL/il.h"
#include "GL/glew.h"


class ModuleTexture : public Module
{
public:

	bool Init() override;

	GLuint Load(const char* path);
	void DeleteTexture(unsigned int textureId);
};

