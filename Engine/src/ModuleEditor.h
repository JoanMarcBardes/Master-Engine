#pragma once
#include "Module.h"
#include "Globals.h"
#include "Libraries/ImGui/imgui.h"
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

private:
	ImVector<char*> Items;
	std::vector<int> fpsLog;
	std::vector<float> msLog;
	int fpsLogSize = 100;

	bool showWindowConfiguration = true;
	bool showWindowConsole = true;
	bool showAbout = false;

	void ClearLog();
	static char* Strdup(const char* s) { size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
	void WindowConsole(bool* p_open);
	void WindowConfiguration(bool* p_open);
	update_status MainMenuBar();
	void WindowAbout(bool* p_open);
	void RequestBrowser(const char* url);
};

