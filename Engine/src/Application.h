#pragma once
#include<vector>
#include<string>
#include "Globals.h"
#include "Module.h"
#include "Timer.h"

class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleEditorCamera;
class ModuleProgram;
class ModuleEditor;
class ModuleDebugDraw;
class ModuleTexture;
class ModuleModel;
class ModuleScene;
class ModuleFilesystem;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	const unsigned int GetFPS() { return fps; }
	const unsigned int GetFpsMax() { return fpsMax; }
	void SetFpsMax(unsigned int FpsMax);

	const char* GetTitle() const { return title.c_str(); }
	const char* GetOrganizatio() const { return organization.c_str(); }

	void SetTitle(const char* Title);
	void SetOrganizatio(const char* Organization) { organization = Organization; }

public:
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleEditorCamera* editorCamera = nullptr;
	ModuleProgram* program = nullptr;
	ModuleRenderExercise* renderExercise = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleDebugDraw* debugDraw = nullptr;
	ModuleTexture* texture = nullptr;
	ModuleModel* model = nullptr;
	ModuleScene* scene = nullptr;
	ModuleFilesystem* filesystem = nullptr;

	bool vsync = true;

private:
	std::vector<Module*> modules;

	unsigned int fps = 0;
	unsigned int fpsCount = 0;

	Timer frameTimer;
	float deltaTime;

	Timer fpsInterval;

	unsigned int fpsMax = 30;
	float msMax = 0.0f;

	std::string title = TITLE;
	std::string organization = ORGANIZATION;

private: 
	void CalculateFPS();

};

extern Application* App;
