#include "Camera.h"
#include "SDL.h"

Camera::Camera(GameObject* ownerGameObject) :
	Component(Component::Type::Camera, ownerGameObject)
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(nearPlane, farPlane);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, aspectRatio);
	frustum.SetPos(float3(0, 2, 6));
	frustum.SetFront(-float3::unitZ);
	frustum.SetUp(float3::unitY);
}

void Camera::Update()
{
}

void Camera::WindowResized(unsigned width, unsigned height)
{
	aspectRatio = (float)width / (float)height;
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, aspectRatio);
}


void Camera::ConstrainFOV()
{
	if (fov > 180.0f)
		fov = 180.0f;
	if (fov < 0.0f)
		fov = 09.0f;
}

void Camera::UpadateCamera()
{
	frustum.SetViewPlaneDistances(nearPlane, farPlane);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, aspectRatio);
}

void Camera::Rotate(const float3x3& rotationMatrix)
{
	vec oldFront = frustum.Front().Normalized();
	vec oldUp = frustum.Up().Normalized();
	frustum.SetFront(rotationMatrix.MulDir(oldFront));
	frustum.SetUp(rotationMatrix.MulDir(oldUp));
}

void Camera::LookAt(const float3& newTarget)
{
	float3 direction = newTarget - frustum.Pos();
	direction.Normalize();

	Rotate(float3x3::LookAt(frustum.Front().Normalized(), direction, frustum.Up().Normalized(), float3::unitY));
}

void Camera::Yaw(float speedYaw)
{
	float3x3 rotationMatrix = frustum.WorldMatrix().RotatePart().RotateY(speedYaw);
	Rotate(rotationMatrix);
}

void Camera::Pitch(float speedPitch) 
{
	vec newFront = (frustum.Front() * cos(speedPitch) + frustum.Up() * sin(speedPitch)).Normalized();
	vec newUp = frustum.WorldRight().Cross(newFront);
	frustum.SetFront(newFront);
	frustum.SetUp(newUp);
}

void Camera::MoveForward(float speedForward)
{
	frustum.Translate(frustum.Front() * speedForward);
	frustum.SetPos(frustum.Pos());
}

void Camera::MoveLateral(float speedLateral) 
{
	frustum.Translate(frustum.WorldRight() * speedLateral);
	frustum.SetPos(frustum.Pos());
}

void Camera::MoveUp(float speedUp)
{
	frustum.SetPos( float3(frustum.Pos().x, frustum.Pos().y + speedUp, frustum.Pos().z) );
}

void Camera::RotateMouse(float speedRotateMouse, iPoint mouse)
{
	Rotate(frustum.WorldMatrix().RotatePart().RotateY(-mouse.x * speedRotateMouse * deltaTime));

	vec newFront = (frustum.Front() * cos(-mouse.y * speedRotateMouse * deltaTime) + frustum.Up() * sin(-mouse.y * speedRotateMouse * deltaTime)).Normalized();
	vec oldUp = frustum.WorldRight().Cross(newFront);
	frustum.SetFront(newFront);
	frustum.SetUp(oldUp);
}

void Camera::WheelMouse(float speedWheelMouse, iPoint mouse_wheel)
{
	fov += mouse_wheel.y * speedWheelMouse;
	ConstrainFOV();
}

void Camera::Focus()
{
	LookAt(target);
}

void Camera::Orbit(float speedOrbit, iPoint mouse)
{
	frustum.Translate(frustum.WorldRight() * -mouse.x * speedOrbit * deltaTime);
	frustum.SetPos(frustum.Pos());

	frustum.Translate(frustum.Front() * mouse.y * speedOrbit * deltaTime);
	frustum.SetPos(frustum.Pos());
		
	frustum.SetPos(float3(frustum.Pos().x, frustum.Pos().y + mouse.y * speedOrbit * deltaTime, frustum.Pos().z));

	LookAt(float3::zero);
}