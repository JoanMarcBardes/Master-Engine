#pragma once
#include<vector>
#include "Globals.h"
#include "Module.h"

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

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	unsigned int GetFPS() { return fps; }

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

private:
	std::vector<Module*> modules;

	unsigned int fps = 0;
	unsigned int fpsCount = 0;
	unsigned int fpsInital = 0;

private: 
	void CalculateFPS();

};

extern Application* App;
