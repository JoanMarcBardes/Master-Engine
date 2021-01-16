#pragma once
#include "Globals.h"
#include "Point.h"
#include "Component.h"
#include "Libraries/MathGeoLib/Geometry/Frustum.h"
#include "Libraries/MathGeoLib/Math/float3x3.h"

class Camera : public Component
{
public:
	Camera(GameObject* ownerGameObject);

	void Update() override;

	void WindowResized(unsigned width, unsigned height);

	void CameraCulling();

	const float4x4 GetProjection() { return frustum.ProjectionMatrix(); }
	const float4x4 GetViewMatrix() { return frustum.ViewMatrix(); }
	const float3   GetPosition()   { return frustum.Pos(); }

	static Type GetType() { return Type::Camera; };
	void SetActive(bool isActive) { active = isActive; }
	bool IsActive() const { return active; };

public:
	Frustum frustum;
	float nearPlane = 0.1f;
	float farPlane = 200.0f;
	float fov = 90;
	float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	bool cullingCam = false;

private:
	bool active = false;
	float3 target = float3::zero;

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	float speed = 0.05;


public:
	void ConstrainFOV();
	void UpadateCamera();

	void Rotate(const float3x3& rotationMatrix);
	void LookAt(const float3& target);

	void Yaw(float speedYaw);
	void Pitch(float speedPitch);
	void MoveForward(float speedForward);
	void MoveLateral(float speedLateral);
	void MoveUp(float speedUp);
	void RotateMouse(float speedRotateMouse, iPoint mouse);
	void WheelMouse(float speedWheelMouse, iPoint mouse_wheel);
	void Focus();
	void Orbit(float speedOrbit, iPoint mouse);

};

