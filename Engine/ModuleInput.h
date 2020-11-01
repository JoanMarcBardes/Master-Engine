#pragma once
#include "Module.h"
#include "Globals.h"
#include "Point.h"
#include "SDL/include/SDL_scancode.h"

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

	bool Init();
	update_status Update();
	bool CleanUp();

	// Check key states (includes mouse and joy buttons)
	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

	// Get mouse / axis position
	const iPoint& GetMouseMotion() const { return mouse_motion;}
	const iPoint& GetMousePosition() const { return mouse; }
	const bool RightMouseOn() const { return right_mouse; }
	const bool LeftMouseOn() const { return left_mouse; }

private:
	KeyState* keyboard;
	KeyState  mouse_buttons[NUM_MOUSE_BUTTONS];
	iPoint mouse_motion;
	iPoint mouse;
	bool right_mouse = false;
	bool left_mouse = false;
};