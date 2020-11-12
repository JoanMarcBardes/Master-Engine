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

	void Load(const char* path);
	const ILuint GetTexId() { return texid; }
	const GLuint GetImage() { return image; }

private:
	ILuint texid;
	GLuint image;
};

