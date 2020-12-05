#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleEditorCamera.h"
#include "ModuleProgram.h"
#include "ModuleRenderExercise.h"
#include "ModuleEditor.h"
#include "ModuleDebugDraw.h"
#include "ModuleTexture.h"
#include "ModuleModel.h"
#include "ModuleScene.h"
#include "Libraries/MathGeoLib/Time/Clock.h"

#include "DebugLeaks.h"

using namespace std;

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.reserve(10);
	modules.push_back(window = new ModuleWindow());
	modules.push_back(texture = new ModuleTexture());
	modules.push_back(model = new ModuleModel());
	modules.push_back(renderExercise = new ModuleRenderExercise());
	modules.push_back(input = new ModuleInput());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(program = new ModuleProgram());
	modules.push_back(debugDraw = new ModuleDebugDraw());
	modules.push_back(editorCamera = new ModuleEditorCamera());
	modules.push_back(scene = new ModuleScene());
}

Application::~Application()
{
	for(vector<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	bool ret = true;

	fpsInital = Clock::Tick();

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	CalculateFPS();

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();		

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(vector<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}

void Application::CalculateFPS() {
	fpsCount++;
	unsigned int fpsInterval = Clock::Tick() - fpsInital;
	if (fpsInterval > Clock::TicksPerSec())
	{
		fps = fpsCount;
		fpsCount = 0;
		fpsInital = Clock::Tick();
	}
}