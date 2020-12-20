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

	const float4x4 GetProjection() { return frustum.ProjectionMatrix(); }
	const float4x4 GetViewMatrix() { return frustum.ViewMatrix(); }
	const float3   GetPosition()   { return position; }

	static Type GetType() { return Type::Camera; };

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

