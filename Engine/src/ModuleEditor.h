#pragma once
#include "Module.h"
#include "Globals.h"
#include "Libraries/ImGui/imgui.h"
#include "ModuleScene.h"
#include "EditorComponent.h"
#include <vector>

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	bool CleanUp() override;

	void AddLog(const char* fmt, ...);	

	void SetActiveWindows(int index, bool active) {
		switch (index)
		{
		case 0:
			showWindowConfiguration = active;
			break;
		case 1:
			showWindowConsole = active;
			break;
		case 2:
			showAbout = active;
			break;
		case 3:
			showWindowGameObjectHierarchy = active;
			break;
		case 4:
			showWindowInspector = active;
			break;
		case 5:
			showWindowPlayStopStep = active;
			break;
		}
	}

private:
	ImVector<char*> Items;
	//std::vector<int> fpsLog;
	//std::vector<float> msLog;
	std::vector<EditorComponent*> components;
	//int fpsLogSize = 100;
	int w = 0, h = 0;
	float menuSize = 0.0f;

	bool showWindowConfiguration = true;
	bool showWindowConsole = true;
	bool showAbout = false;
	bool showWindowGameObjectHierarchy = true;
	bool showWindowInspector = true;
	bool showWindowPlayStopStep = true;
	bool showWindowViewport = true;

	void ClearLog();
	static char* Strdup(const char* s) { size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
	//void WindowConsole(bool* p_open);
	//void WindowConfiguration(bool* p_open);
	//update_status MainMenuBar();
	
	//void WindowAbout(bool* p_open);
	//void RequestBrowser(const char* url);

	void WindowGameObjectHierarchy(bool* p_open);
	void TreeChilds(GameObject* parent);

	GameObject* selected = nullptr;

	void WindowInspector(bool* p_open);

};

