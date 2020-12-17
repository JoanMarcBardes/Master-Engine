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
#include "Time.h"
#include "DebugLeaks.h"
#include "Brofiler.h"

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

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	frameTimer.Start();
	if (fpsMax > 0)
		msMax = (float)1000 / (float)fpsMax;

	return ret;
}

update_status Application::Update()
{
	BROFILER_CATEGORY("Update", Profiler::Color::Orchid);

	update_status ret = UPDATE_CONTINUE;

	deltaTime = frameTimer.GetTimerSec();
	frameTimer.Start();
	Time::PreUpdate(deltaTime);

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();		

	CalculateFPS();
	Time::PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	BROFILER_CATEGORY("CleanUp", Profiler::Color::Orchid);
	bool ret = true;

	for(vector<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}

void Application::CalculateFPS() 
{
	float frame = frameTimer.GetTimer();
	if (frame > 0 && frame < msMax)
	{
		//SDL_Delay(msMax - frame);
	}

	fpsCount++;
	if (fpsInterval.GetTimer() >= 1000)
	{
		fps = fpsCount;
		fpsCount = 0;
		fpsInterval.Start();
	}

}

void Application::SetFpsMax(unsigned int FpsMax)
{
	fpsMax = FpsMax;
	if (fpsMax > 0)
		msMax = (float)1000 / (float)fpsMax;
}

void Application::SetTitle(const char* Title)
{
	title = Title;
	window->SetTitle(Title);
}
