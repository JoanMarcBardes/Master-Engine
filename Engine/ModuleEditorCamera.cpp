#include "ModuleEditorCamera.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "SDL.h"
#include "GL/glew.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Time/Clock.h"
#include "MathGeoLib/Math/float3x3.h"

ModuleEditorCamera::ModuleEditorCamera()
{
}

ModuleEditorCamera::~ModuleEditorCamera()
{
}

bool ModuleEditorCamera::Init()
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 200.0f);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);
	frustum.SetPos(float3(0, 1, -3));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

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
	
	InputMnager();

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
}

void ModuleEditorCamera::InputMnager()
{
	const float cameraSpeed = 0.01f * deltaTime;

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_Q])
	{
		LOG("Key press Q");
		frustum.SetPos(frustum.Pos() + frustum.Up() * cameraSpeed);
	}
	if (keys[SDL_SCANCODE_E])
	{
		LOG("Key press E");
		frustum.SetPos(frustum.Pos() - frustum.Up() * cameraSpeed);
	}
	if (keys[SDL_SCANCODE_W])
	{
		LOG("Key press W");
		frustum.SetPos(frustum.Pos() + frustum.Front() * cameraSpeed);
	}
	if (keys[SDL_SCANCODE_S])
	{
		LOG("Key press S");
		frustum.SetPos(frustum.Pos() - frustum.Front() * cameraSpeed);
	}
	if (keys[SDL_SCANCODE_A])
	{
		LOG("Key press A");
		frustum.SetPos(frustum.Pos() - Cross(frustum.Front(), frustum.Up()).Normalized() * cameraSpeed);
	}
	if (keys[SDL_SCANCODE_D])
	{
		LOG("Key press D");
		frustum.SetPos(frustum.Pos() + Cross(frustum.Front(), frustum.Up()).Normalized() * cameraSpeed);
	}

	if (keys[SDL_SCANCODE_UP])
	{
		LOG("Key press UP");
		++pitch;
		Direction();
	}
	if (keys[SDL_SCANCODE_DOWN])
	{
		LOG("Key press DOWN");
		--pitch;
		Direction();
	}
	if (keys[SDL_SCANCODE_RIGHT])
	{
		LOG("Key press RIGHT");
		++yaw;
		Direction();
	}
	if (keys[SDL_SCANCODE_LEFT])
	{
		LOG("Key press LEFT");
		--yaw;
		Direction();
	}
}

void ModuleEditorCamera::Direction()
{
	ConstrainPitch();

	vec direction;
	direction.x = cos(DEGTORAD * yaw) * cos(DEGTORAD * pitch);
	direction.y = sin(DEGTORAD * pitch);
	direction.z = sin(DEGTORAD * yaw) * cos(DEGTORAD * pitch);
	frustum.SetFront(direction.Normalized());
}

void ModuleEditorCamera::ConstrainPitch()
{
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
}
