#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "SDL/include/SDL.h"
#include "ImGui/imgui_impl_sdl.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput() : Module(), mouse({ 0, 0 }), mouse_motion({ 0,0 }), mouse_wheel({ 0,0 })
{
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	RELEASE_ARRAY(keyboard);
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

	return ret;
}

// Called every draw update
update_status ModuleInput::Update()
{
    SDL_Event sdlEvent;

    mouse_motion = { 0, 0 };
	mouse_wheel = { 0, 0 };

	const Uint8* keys = SDL_GetKeyboardState(NULL);

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
        switch (sdlEvent.type)
        {
            case SDL_QUIT:
                return UPDATE_STOP;
            case SDL_WINDOWEVENT:
                if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED || sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    App->renderer->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
                break;
			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[sdlEvent.button.button - 1] = KEY_DOWN;
				if (sdlEvent.button.button == SDL_BUTTON_LEFT)
					left_mouse = true;
				else if (sdlEvent.button.button == SDL_BUTTON_RIGHT)
					right_mouse = true;
				break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[sdlEvent.button.button - 1] = KEY_UP;
				if (sdlEvent.button.button == SDL_BUTTON_LEFT)
					left_mouse = false;
				else if (sdlEvent.button.button == SDL_BUTTON_RIGHT)
					right_mouse = false;
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
        }

		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
    }


    if (GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
    {
        return UPDATE_STOP;
    }

    return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}
