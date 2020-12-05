#include "ModuleEditorCamera.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "Point.h"
#include "SDL.h"
#include "GL/glew.h"
#include "Libraries/MathGeoLib/Geometry/Frustum.h"
#include "Libraries/MathGeoLib/Time/Clock.h"
#include "Libraries/ImGui/imgui_impl_sdl.h"
#include "DebugLeaks.h"

ModuleEditorCamera::ModuleEditorCamera()
{
	position = float3(0, 2, 6);
}

ModuleEditorCamera::~ModuleEditorCamera()
{
}

bool ModuleEditorCamera::Init()
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(nearPlane, farPlane);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, aspectRatio);
	frustum.SetPos(position);
	frustum.SetFront(front);
	frustum.SetUp(up);

	return true;
}

update_status ModuleEditorCamera::PreUpdate()
{
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleEditorCamera::Update()
{
	float currentFrame = Clock::Time();
	deltaTime = (currentFrame - lastFrame) * 0.01f;
	lastFrame = currentFrame;
	
	MoveForward();
	MoveLateral();
	MoveUp();
	Pitch();
	Yaw();
	RotateMouse();
	WheelMouse();
	Focus();
	Orbit();

	UpadateCamera();

	return UPDATE_CONTINUE;
}

update_status ModuleEditorCamera::PostUpdate()
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleEditorCamera::CleanUp()
{
	LOG("Destroying EditorCamera");

	return true;
}

void ModuleEditorCamera::WindowResized(unsigned width, unsigned height)
{
	aspectRatio = (float)width / (float)height;
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, aspectRatio);
}

void ModuleEditorCamera::ConstrainFOV()
{
	if (fov > 180.0f)
		fov = 180.0f;
	if (fov < 0.0f)
		fov = 09.0f;
}

void ModuleEditorCamera::UpadateCamera()
{
	frustum.SetViewPlaneDistances(nearPlane, farPlane);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, aspectRatio);
	frustum.SetPos(position);
	frustum.SetFront(front);
	frustum.SetUp(up);
}

void ModuleEditorCamera::Rotate(const float3x3& rotationMatrix)
{
	vec oldFront = front.Normalized();
	vec oldUp = up.Normalized();
	front = rotationMatrix.MulDir(oldFront);
	up = rotationMatrix.MulDir(oldUp);
	frustum.SetFront(front);
	frustum.SetUp(up);
}

void ModuleEditorCamera::LookAt(const float3& newTarget)
{
	float3 direction = newTarget - position;
	direction.Normalize();

	Rotate(float3x3::LookAt(front.Normalized(), direction, up.Normalized(), float3::unitY));
}

void ModuleEditorCamera::Yaw() {

	float speedYaw = speed/2;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT)) {
		speedYaw *= 3;
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT)) {
		float3x3 rotationMatrix = frustum.WorldMatrix().RotatePart().RotateY(speedYaw);
		Rotate(rotationMatrix);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT)) {
		float3x3 rotationMatrix = frustum.WorldMatrix().RotatePart().RotateY(-speedYaw);
		Rotate(rotationMatrix);
	}

}

void ModuleEditorCamera::Pitch() {

	float speedPitch = speed/2;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT)) {
		speedPitch *= 3;
	}

	if (App->input->GetKey(SDL_SCANCODE_UP)) {
		vec newFront = (front * cos(speedPitch) + up * sin(speedPitch)).Normalized();
		vec newUp = frustum.WorldRight().Cross(newFront);
		front = newFront;
		up = newUp;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN)) {
		vec newFront = (front * cos(-speedPitch) + up * sin(-speedPitch)).Normalized();
		vec newUp = frustum.WorldRight().Cross(newFront);
		front = newFront;
		up = newUp;
	}
}

void ModuleEditorCamera::MoveForward() {

	float speedForward = speed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT)) {
		speedForward *= 3;
	}

	if (App->input->GetKey(SDL_SCANCODE_W)) {
		frustum.Translate(front * speedForward);
		position = frustum.Pos();
	}

	if (App->input->GetKey(SDL_SCANCODE_S)) {
		frustum.Translate(front * -speedForward);
		position = frustum.Pos();
	}

}

void ModuleEditorCamera::MoveLateral() {

	float speedLateral = speed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT)) {
		speedLateral *= 3;
	}

	if (App->input->GetKey(SDL_SCANCODE_A)) {
		frustum.Translate(frustum.WorldRight() * -speedLateral);
		position = frustum.Pos();
	}

	if (App->input->GetKey(SDL_SCANCODE_D)) {
		frustum.Translate(frustum.WorldRight() * speedLateral);
		position = frustum.Pos();
	}

}

void ModuleEditorCamera::MoveUp() {
	float speedUp = speed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT)) {
		speedUp *= 3;
	}

	if (App->input->GetKey(SDL_SCANCODE_Q)) {
		position.y += speedUp;
	}

	if (App->input->GetKey(SDL_SCANCODE_E)) {
		position.y -= speedUp;
	}
}

void ModuleEditorCamera::RotateMouse() {
	float speedRotateMouse = speed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT)) {
		speedRotateMouse *= 3;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) && !App->input->GetKey(SDL_SCANCODE_LALT)) {
		iPoint mouse = App->input->GetMouseMotion();

		Rotate(frustum.WorldMatrix().RotatePart().RotateY(-mouse.x * speedRotateMouse * deltaTime));

		vec newFront = (front * cos(-mouse.y * speedRotateMouse * deltaTime) + up * sin(-mouse.y * speedRotateMouse * deltaTime)).Normalized();
		vec oldUp = frustum.WorldRight().Cross(newFront);
		front = newFront;
		up = oldUp;
	}
}

void ModuleEditorCamera::WheelMouse() {
	float speedWheelMouse = speed * 100;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT)) {
		speedWheelMouse *= 3;
	}

	iPoint mouse_wheel = App->input->GetMouseWhell();
	if (mouse_wheel.y != 0) {
		fov += mouse_wheel.y * speedWheelMouse;
		ConstrainFOV();
	}
}

void ModuleEditorCamera::Focus() {
	if (App->input->GetKey(SDL_SCANCODE_F)) {
		LookAt(target);
	}
}

void ModuleEditorCamera::Orbit() {
	float speedOrbit = speed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT)) {
		speedOrbit *= 3;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) && App->input->GetKey(SDL_SCANCODE_LALT)) {
		iPoint mouse = App->input->GetMouseMotion();

		frustum.Translate(frustum.WorldRight() * -mouse.x * speedOrbit * deltaTime);
		position = frustum.Pos();

		frustum.Translate(frustum.Front() * mouse.y * speedOrbit * deltaTime);
		position = frustum.Pos();
		position.y += mouse.y * speedOrbit * deltaTime;

		LookAt(float3::zero);
	}
}

void ModuleEditorCamera::AdaptSizeGeometry(float volume)
{
	position = float3(position.x, position.y, volume);
	LookAt(target);
	UpadateCamera();
}