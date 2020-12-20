#include "Camera.h"
#include "SDL.h"

Camera::Camera(GameObject* ownerGameObject) :
	Component(Component::Type::Camera, ownerGameObject)
{
	position = float3(0, 2, 6);
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(nearPlane, farPlane);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, aspectRatio);
	frustum.SetPos(position);
	frustum.SetFront(front);
	frustum.SetUp(up);
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
	frustum.SetPos(position);
	frustum.SetFront(front);
	frustum.SetUp(up);
}

void Camera::Rotate(const float3x3& rotationMatrix)
{
	vec oldFront = front.Normalized();
	vec oldUp = up.Normalized();
	front = rotationMatrix.MulDir(oldFront);
	up = rotationMatrix.MulDir(oldUp);
	frustum.SetFront(front);
	frustum.SetUp(up);
}

void Camera::LookAt(const float3& newTarget)
{
	float3 direction = newTarget - position;
	direction.Normalize();

	Rotate(float3x3::LookAt(front.Normalized(), direction, up.Normalized(), float3::unitY));
}

void Camera::Yaw(float speedYaw)
{
	float3x3 rotationMatrix = frustum.WorldMatrix().RotatePart().RotateY(speedYaw);
	Rotate(rotationMatrix);
}

void Camera::Pitch(float speedPitch) 
{
	vec newFront = (front * cos(speedPitch) + up * sin(speedPitch)).Normalized();
	vec newUp = frustum.WorldRight().Cross(newFront);
	front = newFront;
	up = newUp;	
}

void Camera::MoveForward(float speedForward)
{
	frustum.Translate(front * speedForward);
	position = frustum.Pos();	
}

void Camera::MoveLateral(float speedLateral) 
{
	frustum.Translate(frustum.WorldRight() * speedLateral);
	position = frustum.Pos();
}

void Camera::MoveUp(float speedUp)
{
	position.y += speedUp;
}

void Camera::RotateMouse(float speedRotateMouse, iPoint mouse)
{
	Rotate(frustum.WorldMatrix().RotatePart().RotateY(-mouse.x * speedRotateMouse * deltaTime));

	vec newFront = (front * cos(-mouse.y * speedRotateMouse * deltaTime) + up * sin(-mouse.y * speedRotateMouse * deltaTime)).Normalized();
	vec oldUp = frustum.WorldRight().Cross(newFront);
	front = newFront;
	up = oldUp;
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
	position = frustum.Pos();

	frustum.Translate(frustum.Front() * mouse.y * speedOrbit * deltaTime);
	position = frustum.Pos();
	position.y += mouse.y * speedOrbit * deltaTime;

	LookAt(float3::zero);
}