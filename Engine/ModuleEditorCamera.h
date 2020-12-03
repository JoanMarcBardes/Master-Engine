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
public:
	ModuleEditorCamera();
	~ModuleEditorCamera();

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;
	void WindowResized(unsigned width, unsigned height);

	//get
	const Frustum GetFrustum()	{ return frustum; }
	const float3 GetPosition()	{ return position; }
	const float3 GetFront()		{ return front; }
	const float3 GetUp()		{ return up; }

	const float GetNearPlane()	{ return nearPlane; }
	const float GetFarPlane()	{ return farPlane; }
	const float GetFOV()		{ return fov; }
	const float GetAspectRatio(){ return aspectRatio; }

	const float GetSpeed() { return speed; }

	const float4x4 GetProjection() { return frustum.ProjectionMatrix(); }
	const float4x4 GetViewMatrix() { return frustum.ViewMatrix(); }

	//set
	void SetFrustum(Frustum _frustum)	{ frustum = std::move(_frustum); }
	void SetPosition(float3 _possition)	{ position = std::move(_possition); }
	void SetFront(float3 _front)		{ front = std::move(_front); }
	void SetUp(float3 _up)				{ up = std::move(_up); }

	void SetNearPlane(float _nearPlane)	{ nearPlane = _nearPlane; }
	void SetFarPlane(float _farPlane)	{ farPlane = _farPlane; }
	void SetFOV(float _fov)				{ fov = _fov; ConstrainFOV(); }
	void SetAspectRatio(float _aspectRatio) { aspectRatio = _aspectRatio; }

	void SetSpeed(float _speed) { speed = _speed; }

	void AdaptSizeGeometry(float volume);
	void SetTarget(float3 center) { target = center; }

private:
	Frustum frustum;
	float3 position = float3::zero;
	float3 front = -float3::unitZ;
	float3 up = float3::unitY;
	float3 target = float3::zero;

	float nearPlane = 0.1f;
	float farPlane = 200.0f;
	float fov = 90;
	float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	float speed = 0.05;

	void ConstrainFOV();
	void UpadateCamera();
	void Rotate(const float3x3& rotationMatrix);
	void LookAt(const float3& target);
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

