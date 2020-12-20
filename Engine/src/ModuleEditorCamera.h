#pragma once
#include "Module.h"
#include "Globals.h"
#include "Camera.h"
#include "Libraries/MathGeoLib/Geometry/Frustum.h"
#include "Libraries/MathGeoLib/Math/float3x3.h"

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

	//get
	/*const Frustum GetFrustum()	{ return frustum; }
	const float3 GetPosition()	{ return position; }
	const float3 GetFront()		{ return front; }
	const float3 GetUp()		{ return up; }

	const float GetNearPlane()	{ return nearPlane; }
	const float GetFarPlane()	{ return farPlane; }
	const float GetFOV()		{ return fov; }
	const float GetAspectRatio(){ return aspectRatio; }*/

	const float GetSpeed() { return speed; }
	const Camera* GetCurrentCamera() { return currentCamera; }	

	const float4x4 GetProjection() { return currentCamera->GetProjection(); }
	const float4x4 GetViewMatrix() { return currentCamera->GetViewMatrix();	}
	const float3   GetPosition()   { return currentCamera->GetPosition(); }

	//set
	/*void SetFrustum(Frustum _frustum)	{ frustum = std::move(_frustum); }
	void SetPosition(float3 _possition)	{ position = std::move(_possition); }
	void SetFront(float3 _front)		{ front = std::move(_front); }
	void SetUp(float3 _up)				{ up = std::move(_up); }

	void SetNearPlane(float _nearPlane)	{ nearPlane = _nearPlane; }
	void SetFarPlane(float _farPlane)	{ farPlane = _farPlane; }
	void SetFOV(float _fov)				{ fov = _fov; ConstrainFOV(); }
	void SetAspectRatio(float _aspectRatio) { aspectRatio = _aspectRatio; }*/

	void SetSpeed(float _speed) { speed = _speed; }
	void SetCurrentCamera(Camera* camera) { currentCamera = camera; }

	//void AdaptSizeGeometry(float volume);
	/*void SetTarget(float3 center) { target = center; }*/

private:
	Camera* currentCamera = nullptr;

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

