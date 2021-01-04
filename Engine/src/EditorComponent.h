#pragma once
#include <string>
#include <vector>
#include "SDL.h"
#include "GL/glew.h"
#include "Libraries/ImGui/imgui.h"
#include "Libraries/ImGui/imgui_impl_sdl.h"
#include "Libraries/ImGui/imgui_impl_opengl3.h"
#include "Libraries/MathGeoLib/MathGeoLib.h"

class EditorComponent
{
public:
	enum Type
	{
		None,
		MainMenu,
		Console,
		Config,
		About,
		Viewport,
		Play
	};
public:
	EditorComponent() {};
	virtual ~EditorComponent() {};

	virtual void Draw(int w, int h) {};

	void SetItems(ImVector<char*> itms) { Items = itms; }
	void SetMenuSize(int size) { menuSize = size; }
	void SetActive(bool* isActive) { active = isActive; }

	Type GetType() const { return type; };
	int GetMenuSize() const { return menuSize; };

	bool IsActive() const { return &active; };
	bool IsQuitting() const { return quit; };

protected:
	bool* active = nullptr;
	ImVector<char*> Items;

	Type type = None;
	int menuSize = 0;
	bool quit = false;
};