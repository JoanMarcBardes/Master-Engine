#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Math/float3x3.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleEditorCamera : public Module
{
private:
	Frustum frustum;
	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	float yaw = 90;
	float pitch = 0;
	float fov = 90;
	float aspectRatio = 1;
public:
	ModuleEditorCamera();
	~ModuleEditorCamera();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);

	void InputMnager();
	void Direction();
	void ConstrainPitch();
	void ConstrainFOV();

};

