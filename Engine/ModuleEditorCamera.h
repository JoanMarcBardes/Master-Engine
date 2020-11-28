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

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);

	//get
	const Frustum GetFrustum()	{ return frustum; }
	const float3 GetPosition()	{ return position; }
	const float3 GetFront()		{ return front; }
	const float3 GetRight()		{ return right; }
	const float3 GetUp()		{ return up; }
	const float3 GetWolrdUp()	{ return worldUp; }
	const float3 GetDiection()	{ return direction; }

	const float GetYaw()		{ return yaw; }
	const float GetPitch()		{ return pitch; }	
	const float GetNearPlane()	{ return nearPlane; }
	const float GetFarPlane()	{ return farPlane; }
	const float GetFOV()		{ return fov; }
	const float GetAspectRatio(){ return aspectRatio; }

	const float GetMovementSpeed() { return movementSpeed; }
	const float GetRotationSpeed() { return rotationSpeed; }
	const float GetZoomSpeed()	   { return zoomSpeed; }
	const bool GetOrbit() { return orbit; }

	const float4x4 GetProjection() { return frustum.ProjectionMatrix(); }
	const float4x4 GetViewMatrix() { return frustum.ViewMatrix(); }

	//set
	void SetFrustum(Frustum _frustum)	{ frustum = std::move(_frustum); }
	void SetPosition(float3 _possition)	{ position = std::move(_possition); }
	void SetFront(float3 _front)		{ front = std::move(_front); }
	void SetRight(float3 _right)		{ right = std::move(_right); }
	void SetUp(float3 _up)				{ up = std::move(_up); }
	void SetWolrdUp(float3 _worldUp)	{ worldUp = std::move(_worldUp); }
	void SetDiection(float3 _direction)	{ direction = std::move(_direction); }

	void SetYaw(float _yaw)				{ yaw = _yaw; }
	void SetPitch(float _pitch)			{ pitch = _pitch; }	
	void SetNearPlane(float _nearPlane)	{ nearPlane = _nearPlane; }
	void SetFarPlane(float _farPlane)	{ farPlane = _farPlane; }
	void SetFOV(float _fov)				{ fov = _fov; }
	void SetAspectRatio(float _aspectRatio) { aspectRatio = _aspectRatio; }

	void SetMovementSpeed(float _movementSpeed) { movementSpeed = _movementSpeed; }
	void SetRotationSpeed(float _rotationSpeed) { rotationSpeed = _rotationSpeed; }
	void SetZoomSpeed(float _zoomSpeed) { zoomSpeed = _zoomSpeed; }
	void SetOrbit(bool _orbit) { orbit = _orbit; }

private:
	Frustum frustum;
	float3 position = float3::zero;
	float3 front = -float3::unitZ;
	float3 up = float3::unitY;
	float3 direction;
	float3 right;
	float3 worldUp;
	float3 target = float3::zero;

	float yaw = -90;
	float pitch = 0;
	float nearPlane = 0.1f;
	float farPlane = 200.0f;
	float fov = 90;
	float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	float movementSpeed = 0.0f;
	float rotationSpeed = 0.0f;
	float zoomSpeed = 0.0f;

	bool orbit = false;

	void ConstrainPitch();
	void ConstrainFOV();
	void InputManager();
	void UpdateAplayYawPitch(float xOffset, float yOffset);
	void UpadateCamera();
	void Rotate(const float3x3& rotationMatrix);
	void LookAt(const float3& target);
	void Print();
};

