#include "ModuleEditorCamera.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "Point.h"
#include "SDL.h"
#include "GL/glew.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Time/Clock.h"
#include "ImGui/imgui_impl_sdl.h"
#include "DebugLeaks.h"

ModuleEditorCamera::ModuleEditorCamera()
{
	direction = (position - float3::zero).Normalized();
	right = Cross(up, direction).Normalized();
	worldUp = Cross(direction, right).Normalized();
}

ModuleEditorCamera::~ModuleEditorCamera()
{
}

bool ModuleEditorCamera::Init()
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(nearPlane, farPlane);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, aspectRatio);
	position = float3(0, 1, 4);
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
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	
	InputManager();
	
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
	LOG("WindowResized EditorCamera");
	aspectRatio = (float)width / (float)height;
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, aspectRatio);

	//App->window->SetWidthHeight(width, height);
}

void ModuleEditorCamera::InputManager()
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard || io.WantCaptureMouse)
		return;

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	float speed = 1;
	bool alt = false;

	if (keys[SDL_SCANCODE_LALT])
		alt = true;

	if (keys[SDL_SCANCODE_LSHIFT])
		speed = 2;

	movementSpeed = 0.005f * deltaTime * speed;
	rotationSpeed = 0.05f * deltaTime * speed;
	zoomSpeed = 0.05f * deltaTime * speed;

	if (keys[SDL_SCANCODE_Q])
		position += up * movementSpeed;

	if (keys[SDL_SCANCODE_E])
		position -= up * movementSpeed;

	if (keys[SDL_SCANCODE_W])
		position += front * movementSpeed;

	if (keys[SDL_SCANCODE_S])
		position -= front * movementSpeed;

	if (keys[SDL_SCANCODE_A])
		position -= Cross(front, up).Normalized() * movementSpeed;

	if (keys[SDL_SCANCODE_D])
		position += Cross(front, up).Normalized() * movementSpeed;

	if (keys[SDL_SCANCODE_UP])
		UpdateAplayYawPitch(0, 1);

	if (keys[SDL_SCANCODE_DOWN])
		UpdateAplayYawPitch(0, -1);

	if (keys[SDL_SCANCODE_RIGHT])
		UpdateAplayYawPitch(1, 0);

	if (keys[SDL_SCANCODE_LEFT])
		UpdateAplayYawPitch(-1, 0);
		
	iPoint mouse_motion = App->input->GetMouseMotion();
	//Drag camera
	if (App->input->LeftMouseOn() && !alt) {
		if (mouse_motion.x != 0)
			position -= mouse_motion.x * Cross(front, up).Normalized() * movementSpeed;
		if (mouse_motion.y != 0)
			position -= mouse_motion.y *front * movementSpeed;
	}
	//Orbit
	else if (App->input->LeftMouseOn() && alt) {
		UpdateAplayYawPitch(mouse_motion.x, -mouse_motion.y);
	}
	//Zoom
	else if (App->input->RightMouseOn() && alt) {
		if (mouse_motion.x != 0)
			fov += mouse_motion.x * zoomSpeed;
		if (mouse_motion.y != 0)
			fov += mouse_motion.y * zoomSpeed;
		ConstrainFOV();
	}

	iPoint mouse_wheel = App->input->GetMouseWhell();
	if (mouse_wheel.y != 0) {
		position -= mouse_wheel.y * front * zoomSpeed;
	}


	if (keys[SDL_SCANCODE_F]) {
		LookAt(float3::zero);
	}

	if (keys[SDL_SCANCODE_P]) {
		LOG("PRESED P");
		Print();
	}
}

void ModuleEditorCamera::ConstrainPitch()
{
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
}

void ModuleEditorCamera::ConstrainFOV()
{
	if (fov > 180.0f)
		fov = 180.0f;
	if (fov < 0.0f)
		fov = 09.0f;
}

void ModuleEditorCamera::UpdateAplayYawPitch(float xOffset, float yOffset)
{
	yaw += xOffset * rotationSpeed;
	pitch += yOffset * rotationSpeed;
	ConstrainPitch();

	vec auxFront;
	auxFront.x = cos(DEGTORAD * yaw) * cos(DEGTORAD * pitch);
	auxFront.y = sin(DEGTORAD * pitch);
	auxFront.z = sin(DEGTORAD * yaw) * cos(DEGTORAD * pitch);

	front = auxFront.Normalized();
	right = Cross(front, worldUp).Normalized();
	up = Cross(right, front).Normalized();
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
	front = rotationMatrix * oldFront;
	up = rotationMatrix * oldUp;	
}

void ModuleEditorCamera::LookAt(const float3& target)
{
	vec direction = target - position;
	direction.Normalize();

	/*pitch = asin(direction.y / direction.Length());
	yaw = asin(direction.x / (cos(pitch) * direction.Length()));
	//pitch = asin(-direction.y);
	//yaw = atan2(direction.x, direction.z);
	vec auxFront;
	auxFront.x = cos(DEGTORAD * yaw) * cos(DEGTORAD * pitch);
	auxFront.y = sin(DEGTORAD * pitch);
	auxFront.z = sin(DEGTORAD * yaw) * cos(DEGTORAD * pitch);

	front = auxFront.Normalized();
	right = Cross(front, worldUp).Normalized();
	up = Cross(right, front).Normalized();*/

	Rotate(float3x3::LookAt(front.Normalized(), direction, up.Normalized(), float3::unitY));
}

void ModuleEditorCamera::Print()
{
	LOG("Psition: %f, %f, %f", position.x, position.y, position.z);
	LOG("front: %f, %f, %f", front.x, front.y, front.z);
	LOG("up: %f, %f, %f", up.x, up.y, up.z);
	LOG("FOV: %f", fov);
	LOG("aspectRatio: %f", aspectRatio);
	LOG("nearPlane: %f", nearPlane);
	LOG("farPlane: %f", farPlane);
}
