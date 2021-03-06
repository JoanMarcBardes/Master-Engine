#pragma once
#include "Module.h"
#include "Globals.h"
#include "Point.h"
#include "Libraries/SDL/include/SDL_scancode.h"

#define NUM_MOUSE_BUTTONS 5

typedef unsigned __int8 Uint8;

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:	
	ModuleInput();
	~ModuleInput();

	bool Init() override;
	update_status PreUpdate() override;
	bool CleanUp() override;

	// Check key states (includes mouse and joy buttons)
	KeyState GetKey(int id) const;

	KeyState GetMouseButtonDown(int id) const;

	// Get mouse / axis position
	const iPoint& GetMouseMotion() const { return mouse_motion;}
	const iPoint& GetMousePosition() const { return mouse; }
	const iPoint& GetMouseWhell() const { return mouse_wheel; }
	const bool RightMouseOn() const { return right_mouse; }
	const bool LeftMouseOn() const { return left_mouse; }

	void SetImGuiOn(bool b) { imgUiOn = b; };

	const std::string GetBasePath() const { return basePath; }

private:
	KeyState* keyboard;
	KeyState  mouse_buttons[NUM_MOUSE_BUTTONS];
	iPoint mouse_motion;
	iPoint mouse;
	iPoint mouse_wheel;
	bool right_mouse = false;
	bool left_mouse = false;
	bool imgUiOn = false;

	std::string _currentDir;
	std::string basePath;
};