#pragma once
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"
#include "Libraries/MathGeoLib/Math/float4x4.h"
#include <string>

class ModuleRenderExercise : public Module
{
public:
	ModuleRenderExercise();
	~ModuleRenderExercise();

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;
	void WindowResized(unsigned width, unsigned height);

	//get
	void* GetContext() const { return _context; }
	bool GetEnableDepthTest() { return enableDepthTest; }
	bool GetEnableCullFace() { return enableCullFace; }
	bool GetEnableAlphaTest() { return enableAlphaTest; }
	float4 GetBackground() { return background; }

	float3 GetLightDir() { return light_dir; }
	float4 GetLightColor() { return light_color; }
	float4 GetAmbientColor() { return ambient_color; }
	float GetKs() { return Ks; }
	float GetKd() { return Kd; }
	int GetShininess() { return shininess; }

	// set
	void SetEnableDepthTest(const bool EnableDepthTest) { enableDepthTest = EnableDepthTest; SetGlEnable(enableDepthTest, GL_DEPTH_TEST); }
	void SetEnableCullFace(const bool EnableCullFace) { enableCullFace = EnableCullFace; SetGlEnable(enableCullFace, GL_CULL_FACE); }
	void SetEnableAlphaTest(const bool EnableAlphaTest) { enableAlphaTest = EnableAlphaTest; SetGlEnable(enableAlphaTest, GL_ALPHA_TEST); }
	void SetGlEnable(const bool enable, const GLenum type);
	void SetBackground(const float4 Background) { background = Background; }

	void SetLightDir(const float3 lightDir) { light_dir = lightDir; }
	void SetLightColor(const float4 lightColor) { light_color = lightColor; }
	void SetAmbientColor(const float4 ambientColor) { ambient_color = ambientColor; }
	void SetKs(const float ks) { Ks = ks; }
	void SetKd(const float kd) { Kd = kd; }
	void SetShininess(const int Shinines) { shininess = Shinines; }

private:
	unsigned _vboQuad = 0;
	unsigned _vboTriangle = 0;
	void* _context = nullptr;
	unsigned _program = 0;
	std::string _currentDir;
	bool enableDepthTest = true;
	bool enableCullFace = true;
	bool enableAlphaTest = true;
	float4 background = float4(0.1f, 0.1f, 0.1f, 1.0f);

	float3 light_dir = float3(1, 0, 0);
	float4 light_color = float4(1, 1, 1, 1);
	float4 ambient_color = float4(0.1f, 0.1f, 0.1f, 1.0f);
	float Ks = 0.7f;
	float Kd = 0.6f;
	int shininess = 70;

	void CreateTriangleVBO();
	void CreateQuadVBO();
	void LoadMeshes();
	void DestroyVBO(unsigned vbo);
	void Draw();
	void DrawTriangle(const float4x4& proj, const float4x4& view);
	void DrawQuad(const float4x4& proj, const float4x4& view);
	void DrawMesh(const float4x4& proj, const float4x4& view, const float4x4& model);
	void DropFile();
};

