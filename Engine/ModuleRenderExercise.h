#pragma once
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"
#include "MathGeoLib/Math/float4x4.h"
#include <string>

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
	void WindowResized(unsigned width, unsigned height);

	void CreateTriangleVBO();
	void CreateQuadVBO();
	void LoadMeshes();
	void DestroyVBO(unsigned vbo);
	void Draw();
	void DrawTriangle(const float4x4& proj, const float4x4& view);
	void DrawQuad(const float4x4& proj, const float4x4& view);
	void DrawMesh(const float4x4& proj, const float4x4& view, const float4x4& model);
	void DropFile();


	void* GetContext() const { return _context; }
	bool GetEnableDepthTest() { return enableDepthTest; }
	bool GetEnableCullFace() { return enableCullFace; }
	bool GetEnableAlphaTest() { return enableAlphaTest; }

	void SetEnableDepthTest(const bool EnableDepthTest) { enableDepthTest = EnableDepthTest; SetGlEnable(enableDepthTest, GL_DEPTH_TEST); }
	void SetEnableCullFace(const bool EnableCullFace) { enableCullFace = EnableCullFace; SetGlEnable(enableCullFace, GL_CULL_FACE); }
	void SetEnableAlphaTest(const bool EnableAlphaTest) { enableAlphaTest = EnableAlphaTest; SetGlEnable(enableAlphaTest, GL_ALPHA_TEST); }
	void SetGlEnable(const bool enable, const GLenum type);

private:
	unsigned _vboQuad = 0;
	unsigned _vboTriangle = 0;

	void* _context = nullptr;
	unsigned _program = 0;
	std::string _currentDir;

	bool enableDepthTest = true;
	bool enableCullFace = true;
	bool enableAlphaTest = true;
};

