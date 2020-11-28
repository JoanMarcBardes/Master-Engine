#pragma once
#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	virtual ~ModuleWindow();

	bool Init();
	update_status Update();
	bool CleanUp();

	//get
	const float GetBrightness() const { return SDL_GetWindowBrightness(window); }
	const int GetWidth() const { return width; }
	const int GetHeight() const { return height; }

	const bool GetResizable() const { return resizable; }
	const bool GetBorderless() const { return bordered; }
	const Uint32 GetWindowFull()const { return windowsFull; }
	
	//set
	void SetBrightness(const float brightness) { SDL_SetWindowBrightness(window, brightness); }
	void SetWidth(const int w) { width = w; SDL_SetWindowSize(window, width, height); }
	void SetHeight(const int h) { height = h; SDL_SetWindowSize(window, width, height); }
	void SetWidthHeight(const int w, const int h);

	void SetFullScreen(const bool active) { windowsFull = active ? SDL_WINDOW_FULLSCREEN : 0; SDL_SetWindowFullscreen(window, windowsFull); }
	void SetResizable(const bool active) { resizable = active; SDL_SetWindowResizable(window, active ? SDL_TRUE : SDL_FALSE); }
	void SetBorderless(const bool active) { bordered = active; SDL_SetWindowBordered(window, active ? SDL_TRUE : SDL_FALSE); }
	void SetFullDesktop(const bool active) { windowsFull = active ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0; SDL_SetWindowFullscreen(window, windowsFull); }

public:
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screen_surface = NULL;

private:
	int width;
	int height;
	bool resizable = true;
	bool bordered = true;
	Uint32 windowsFull = 0;
};