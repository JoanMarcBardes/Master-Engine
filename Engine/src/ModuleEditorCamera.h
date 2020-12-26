#pragma once
#include "Module.h"
#include "Globals.h"
#include "Camera.h"
#include "Libraries/MathGeoLib/Geometry/Frustum.h"
#include "Libraries/MathGeoLib/Math/float3x3.h"
#include <vector>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleEditorCamera : public Module
{
public:
	ModuleEditorCamera();
	~ModuleEditorCamera();

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;
	void WindowResized(unsigned width, unsigned height);

	const float GetSpeed() { return speed; }
	Camera* GetCurrentCamera() { return currentCamera; }	

	const float4x4 GetProjection() { return currentCamera->GetProjection(); }
	const float4x4 GetViewMatrix() { return currentCamera->GetViewMatrix();	}
	const float3   GetPosition()   { return currentCamera->GetPosition(); }

	void SetSpeed(float _speed) { speed = _speed; }
	void SetCurrentCamera(Camera* camera);
	void AddCamera(Camera* camera, bool setAsCurrentCamera = false);
	void SetActiveCamera(Camera* camera, bool active);
	std::vector<Camera*> GetAllCameras() { return allCameras; }

	//void AdaptSizeGeometry(float volume);
	/*void SetTarget(float3 center) { target = center; }*/

private:
	Camera* currentCamera = nullptr;
	std::vector<Camera*> allCameras;

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	float speed = 0.05;

	void UpadateCamera();
	void Yaw();
	void Pitch();
	void MoveForward();
	void MoveLateral();
	void MoveUp();
	void RotateMouse();
	void WheelMouse();
	void Focus();
	void Orbit();
};

