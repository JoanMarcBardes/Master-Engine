#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleModel.h"
#include "ModuleTexture.h"
#include "Libraries/SDL/include/SDL.h"
#include "Libraries/ImGui/imgui_impl_sdl.h"
#include "DebugLeaks.h"
#include <string>
#include <direct.h>

using namespace std;

#define MAX_KEYS 300
std::string get_current_dir() {
	char buff[FILENAME_MAX]; //create string buffer to hold path
	_getcwd(buff, FILENAME_MAX);
	string current_dir(buff);
	return current_dir;
}
void ReplaceSlash(string& str)
{
	string oldStr = "\\";
	string newStr = "/";
	size_t index = 0;
	while (true) {
		index = str.find(oldStr, index);
		if (index == string::npos) break;

		str.replace(index, oldStr.length(), newStr);

		index += 3; //Advance index forward so the next iteration doesn't pick it up as well.
	}
}

ModuleInput::ModuleInput() : Module(), mouse({ 0, 0 }), mouse_motion({ 0,0 }), mouse_wheel({ 0,0 })
{
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	CleanUp();
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;

	SDL_Init(0);
	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	// Current directory
	_currentDir = get_current_dir() + "\\";
	ReplaceSlash(_currentDir);
	basePath = SDL_GetBasePath();

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate()
{
    SDL_Event sdlEvent;

    mouse_motion = { 0, 0 };
	mouse_wheel = { 0, 0 };
	//memset(windowEvents, false, WE_COUNT * sizeof(bool));

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keyboard[SDL_SCANCODE_ESCAPE] == KEY_DOWN)
	{
		return UPDATE_STOP;
	}

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if (mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}
	
    while (SDL_PollEvent(&sdlEvent) != 0)
    {
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);	

		ImGuiIO& io = ImGui::GetIO();
		imgUiOn = io.WantCaptureKeyboard || io.WantCaptureMouse;

        switch (sdlEvent.type)
        {
            case SDL_QUIT:
                return UPDATE_STOP;
			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[sdlEvent.button.button - 1] = KEY_DOWN;
				break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[sdlEvent.button.button - 1] = KEY_UP;
				break;

			case SDL_MOUSEMOTION:
				mouse_motion.x = sdlEvent.motion.xrel / SCREEN_SIZE;
				mouse_motion.y = sdlEvent.motion.yrel / SCREEN_SIZE;
				mouse.x = sdlEvent.motion.x / SCREEN_SIZE;
				mouse.y = sdlEvent.motion.y / SCREEN_SIZE;
				break;

			case SDL_MOUSEWHEEL:
				mouse_wheel.x = sdlEvent.wheel.x;
				mouse_wheel.y = sdlEvent.wheel.y;
				break;

			case (SDL_DROPFILE):      // In case if dropped file
				char* dropped_filedir = sdlEvent.drop.file;
				string s(dropped_filedir);
				ReplaceSlash(s);
				if (s.find(".fbx") < s.length() || s.find(".FBX") < s.length() || s.find(".obj") < s.length())
				{
					LOG( ("Loading model " + s).c_str());
					App->model->Load(s.c_str());
				}
				else if (s.find(".png") < s.length() || s.find(".jpg") < s.length() || s.find(".dds") < s.length() || s.find(".tga") < s.length())
				{
					LOG(("Loading texture " + s).c_str());
					App->model->SetTexture( App->texture->Load(s.c_str()), s.c_str());
				}
				else
				{
					LOG( (s +" its not a file .fbx, .png, .jpg, .dds or .tga").c_str() );
				}
				SDL_free(dropped_filedir);    // Free dropped_filedir memory
				break;		
        }
		
    }
	    
    return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	RELEASE_ARRAY(keyboard);
	return true;
}

KeyState ModuleInput::GetKey(int id) const
{
	if (imgUiOn)
		return KEY_IDLE;

	return keyboard[id];
}

KeyState ModuleInput::GetMouseButtonDown(int id) const
{
	if (imgUiOn)
		return KEY_IDLE;

	return mouse_buttons[id - 1];
}