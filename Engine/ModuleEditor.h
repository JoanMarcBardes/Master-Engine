#pragma once
#include "Module.h"
#include "Globals.h"
#include "ImGui/imgui.h"
#include <list>
#include <vector>

class ModuleEditor : public Module
{
public:
	ImVector<char*> Items;

	ModuleEditor();
	~ModuleEditor();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void WindowHello();

	void WindowConsole();
	void ClearLog();
	void AddLog(const char* fmt, ...);
	static char* Strdup(const char* s) { size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
	void WindowConfiguration();
	void MainMenuBar();
	void RequestBrowser(const char* url);

private:
	std::vector<int> fpsLog;
	int fpsLogSize = 100;
};

