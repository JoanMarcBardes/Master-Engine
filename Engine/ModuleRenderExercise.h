#pragma once
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"

class ModuleRenderExercise : public Module
{
public:
	ModuleRenderExercise();
	~ModuleRenderExercise();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	unsigned CreateTriangleVBO();
	void DestroyVBO(unsigned vbo);
	void RenderVBO(unsigned vbo, unsigned program);

private:
	unsigned _vbo;
	void* _context;
	unsigned _program;
};

