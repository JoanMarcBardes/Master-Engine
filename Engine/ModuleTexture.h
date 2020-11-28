#pragma once
#include "Module.h"
#include "Globals.h"
#include "DevIL-SDK/include/IL/il.h"
#include "GL/glew.h"


class ModuleTexture : public Module
{
public:
	ModuleTexture();
	~ModuleTexture();

	bool Init();
	bool CleanUp();

	GLuint Load(const char* path);

	void DeleteTexture(unsigned int textureId);
};

