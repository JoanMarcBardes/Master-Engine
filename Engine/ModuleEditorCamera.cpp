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

ModuleEditorCamera::ModuleEditorCamera()
{
	aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	position = float3(0, 0, -3);
	front = float3::unitZ;
	up = float3::unitY;
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
	float4x4 projectionGL = frustum.ProjectionMatrix().Transposed(); //<-- Important to transpose!
	float4x4 viewMatrix = frustum.ViewMatrix();
	float4x4 viewProjGL = viewMatrix.Transposed(); //<-- Important to transpose!

	//Send the frustum projection matrix to OpenGL
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(*projectionGL.v);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(*viewProjGL.v);
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
	fov = (height - App->window->height) != 0 ? fov * aspectRatio : fov;

	ConstrainFOV();

	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, aspectRatio);

	App->window->width = width;
	App->window->height = height;
}

void ModuleEditorCamera::InputManager()
{
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	float speed = 1;
	bool ctrl = false;
	bool alt = false;

	if (keys[SDL_SCANCODE_LCTRL])
		ctrl = true;

	if (keys[SDL_SCANCODE_LALT])
		alt = true;

	if (keys[SDL_SCANCODE_LSHIFT])
		speed = 2;

	movementSpeed = 0.01f * deltaTime * speed;
	rotationSpeed = 0.1f * deltaTime * speed;

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
	if (App->input->LeftMouseOn() && ctrl) {
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
			fov += mouse_motion.x * movementSpeed * 5;
		if (mouse_motion.y != 0)
			fov += mouse_motion.y * movementSpeed * 5;
		ConstrainFOV();
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
