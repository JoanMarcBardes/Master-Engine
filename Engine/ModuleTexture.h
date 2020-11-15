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
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	GLuint Load(const char* path);

	const GLuint GetTexture() { return texture; }

private:	
	GLuint texture;
};

