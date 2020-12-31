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
	unsigned GetProgram() const { return _program; }
	bool GetEnableDepthTest() { return enableDepthTest; }
	bool GetEnableCullFace() { return enableCullFace; }
	bool GetEnableAlphaTest() { return enableAlphaTest; }
	float4 GetBackground() { return background; }

	float3 GetLightDir() { return light_dir; }
	float3 GetLightColor() { return light_color; }
	float3 GetAmbientColor() { return ambient_color; }
	int GetShininess() { return shininess; }
	float3 GetDiffuseColor() { return diffuse_color; }
	float3 GetSpecularColor() { return specular_color; }
	int GetHasDiffuseMap() { return has_diffuse_map; }
	int GetHasSpecularMap() { return has_specular_map; }
	int GetShininessApha() { return shininess_alpha; }

	// set
	void SetEnableDepthTest(const bool EnableDepthTest) { enableDepthTest = EnableDepthTest; SetGlEnable(enableDepthTest, GL_DEPTH_TEST); }
	void SetEnableCullFace(const bool EnableCullFace) { enableCullFace = EnableCullFace; SetGlEnable(enableCullFace, GL_CULL_FACE); }
	void SetEnableAlphaTest(const bool EnableAlphaTest) { enableAlphaTest = EnableAlphaTest; SetGlEnable(enableAlphaTest, GL_ALPHA_TEST); }
	void SetGlEnable(const bool enable, const GLenum type);
	void SetBackground(const float4 Background) { background = Background; }

	void SetLightDir(const float3 lightDir) { light_dir = lightDir; }
	void SetLightColor(const float3 lightColor) { light_color = lightColor; }
	void SetAmbientColor(const float3 ambientColor) { ambient_color = ambientColor; }
	void SetShininess(const int Shinines) { shininess = Shinines; }
	void SetDiffuseColor(const float3 diffuseColor) { diffuse_color = diffuseColor; }
	void SetSpecularColor(const float3 specularColor) { specular_color = specularColor; }
	void SetHasDiffuseMap(const int hasDiffuseMap) { has_diffuse_map = hasDiffuseMap; }
	void SetHasSpecularMap(const int hasSpecularMap) { has_specular_map = hasSpecularMap; }
	void SetShininessApha(const int shininessAlpha) { shininess_alpha = shininessAlpha; }

private:
	unsigned _vboQuad = 0;
	unsigned _vboTriangle = 0;
	void* _context = nullptr;
	unsigned _program = 0;
	bool enableDepthTest = true;
	bool enableCullFace = true;
	bool enableAlphaTest = true;
	float4 background = float4(0.1f, 0.1f, 0.1f, 1.0f);

	float3 light_dir = float3(1, 0, 0);
	float3 light_color = float3(1, 1, 1);
	float3 ambient_color = float3(0.1f, 0.1f, 0.1f);
	int shininess = 70;
	float3 diffuse_color = float3(1.0f, 0.1f, 0.1f);
	float3 specular_color = float3(0.08f, 0.08f, 0.08f);
	int has_diffuse_map = 1;
	int has_specular_map = 1;
	int shininess_alpha = 1;


	void CreateTriangleVBO();
	void CreateQuadVBO();
	void LoadMeshes();
	void DestroyVBO(unsigned vbo);
	void Draw();
	void DrawTriangle(const float4x4& proj, const float4x4& view);
	void DrawQuad(const float4x4& proj, const float4x4& view);
	void DrawMesh(const float4x4& proj, const float4x4& view, const float4x4& model);
};

