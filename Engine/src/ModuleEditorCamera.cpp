#include "ModuleEditorCamera.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "Point.h"
#include "SDL.h"
#include "GL/glew.h"
#include "Libraries/MathGeoLib/Geometry/Frustum.h"
#include "Libraries/ImGui/imgui_impl_sdl.h"
#include "Time.h"
#include "DebugLeaks.h"

ModuleEditorCamera::ModuleEditorCamera()
{
}

ModuleEditorCamera::~ModuleEditorCamera()
{
}

bool ModuleEditorCamera::Init()
{
	return true;
}

// Called every draw update
update_status ModuleEditorCamera::Update()
{
	deltaTime = Time::deltaTime;
	
	MoveForward();
	MoveLateral();
	MoveUp();
	Pitch();
	Yaw();
	RotateMouse();
	MoveMouse();
	WheelMouse();
	Focus();
	Orbit();

	UpadateCamera();

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
	currentCamera->WindowResized(width, height);
}

void ModuleEditorCamera::UpadateCamera()
{
	currentCamera->UpadateCamera();
}

void ModuleEditorCamera::Yaw() 
{

	float speedYaw = speed/2;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT))
		speedYaw *= 3;

	if (App->input->GetKey(SDL_SCANCODE_LEFT))
		currentCamera->Yaw(speedYaw);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT))
		currentCamera->Yaw(-speedYaw);

}

void ModuleEditorCamera::Pitch()
{
	float speedPitch = speed/2;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT))
		speedPitch *= 3;

	if (App->input->GetKey(SDL_SCANCODE_UP))
		currentCamera->Pitch(speedPitch);

	if (App->input->GetKey(SDL_SCANCODE_DOWN))
		currentCamera->Pitch(-speedPitch);
}

void ModuleEditorCamera::MoveForward()
{
	float speedForward = speed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT))
		speedForward *= 3;

	if (App->input->GetKey(SDL_SCANCODE_W))
		currentCamera->MoveForward(speedForward);

	if (App->input->GetKey(SDL_SCANCODE_S))
		currentCamera->MoveForward(-speedForward);

}

void ModuleEditorCamera::MoveLateral()
{
	float speedLateral = speed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT))
		speedLateral *= 3;

	if (App->input->GetKey(SDL_SCANCODE_A))
		currentCamera->MoveLateral(-speedLateral);

	if (App->input->GetKey(SDL_SCANCODE_D))
		currentCamera->MoveLateral(speedLateral);
}

void ModuleEditorCamera::MoveUp()
{
	float speedUp = speed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT))
		speedUp *= 3;

	if (App->input->GetKey(SDL_SCANCODE_Q))
		currentCamera->MoveUp(speedUp);

	if (App->input->GetKey(SDL_SCANCODE_E))
		currentCamera->MoveUp(-speedUp);
}

void ModuleEditorCamera::RotateMouse()
{
	float speedRotateMouse = speed / 4;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT))
		speedRotateMouse *= 3;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) && !App->input->GetKey(SDL_SCANCODE_LALT)) {
		iPoint mouse = App->input->GetMousePosition();
		
		if (mouse.x > preMousePosX) {
			currentCamera->Yaw(-speedRotateMouse);
			preMousePosX = mouse.x;
		}else if (mouse.x < preMousePosX){
			currentCamera->Yaw(speedRotateMouse);
			preMousePosX = mouse.x;
		}
		if (mouse.y > preMousePosY) {
			currentCamera->Pitch(-speedRotateMouse);
			preMousePosY = mouse.y;
		}else if (mouse.y < preMousePosY) {
			currentCamera->Pitch(speedRotateMouse);
			preMousePosY = mouse.y;
		}
		//currentCamera->RotateMouse(speedRotateMouse, mouse);
	}
}

void ModuleEditorCamera::MoveMouse()
{
	float speedMoveMouse = speed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT))
		speedMoveMouse *= 3;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) && !App->input->GetKey(SDL_SCANCODE_LALT)) {
		iPoint mouse = App->input->GetMousePosition();

		if (mouse.x > preMousePosX) {
			currentCamera->MoveLateral(-speedMoveMouse);
			preMousePosX = mouse.x;
		}
		else if (mouse.x < preMousePosX) {
			currentCamera->MoveLateral(speedMoveMouse);
			preMousePosX = mouse.x;
		}
		if (mouse.y > preMousePosY) {
			currentCamera->MoveUp(speedMoveMouse / 2);
			preMousePosY = mouse.y;
		}
		else if (mouse.y < preMousePosY) {
			currentCamera->MoveUp(-speedMoveMouse / 2);
			preMousePosY = mouse.y;
		}
		//currentCamera->RotateMouse(speedRotateMouse, mouse);
	}
}

void ModuleEditorCamera::WheelMouse()
{
	float speedWheelMouse = speed * 100;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT))
		speedWheelMouse *= 3;

	iPoint mouse_wheel = App->input->GetMouseWhell();
	if (mouse_wheel.y != 0)
		currentCamera->WheelMouse(speedWheelMouse, mouse_wheel);
}

void ModuleEditorCamera::Focus()
{
	if (App->input->GetKey(SDL_SCANCODE_F))
		currentCamera->Focus();
}

void ModuleEditorCamera::Orbit()
{
	float speedOrbit = 2.0f;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT))
		speedOrbit *= 3;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) && App->input->GetKey(SDL_SCANCODE_LALT)) {
		iPoint mouse = App->input->GetMousePosition();

		if (mouse.x > preMousePosX) {
			currentCamera->Orbit(speedOrbit, mouse);
			preMousePosX = mouse.x;
		}
		else if(mouse.x < preMousePosX) {
			currentCamera->Orbit(-speedOrbit, mouse);
			preMousePosX = mouse.x;
		}

	}
}

/*void ModuleEditorCamera::AdaptSizeGeometry(float volume)
{
	position = float3(position.x, position.y, volume);
	currentCamera->LookAt(target);
	UpadateCamera();
}*/

void ModuleEditorCamera::SetCurrentCamera(Camera* camera)
{
	currentCamera = camera;
}

void ModuleEditorCamera::RemoveAllCameras()
{
	allCameras.clear();
	currentCamera = nullptr;
}

void ModuleEditorCamera::AddCamera(Camera* camera, bool setAsCurrentCamera)
{
	allCameras.push_back(camera);
	if (setAsCurrentCamera)
		SetActiveCamera(camera,true);
}

void ModuleEditorCamera::SetActiveCamera(Camera* camera, bool active)
{
	if (active)
	{
		for each (Camera * cam in allCameras)
		{
			cam->SetActive(false);
		}
		camera->SetActive(true);
		SetCurrentCamera(camera);
	}
	else
	{
		bool anyCamera = false;
		for each (Camera * cam in allCameras)
		{
			if (cam != camera)
			{
				cam->SetActive(true);
				SetCurrentCamera(cam);
				anyCamera = true;
				break;
			}
		}
		camera->SetActive(false);

		if(!anyCamera)
			SetCurrentCamera(nullptr);
	}
}
