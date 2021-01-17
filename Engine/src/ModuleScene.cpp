#include "ModuleScene.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEditorCamera.h"
#include "ModuleInput.h"
#include "ImporterScene.h"
#include "ModuleFilesystem.h"
#include "ModuleEditor.h"
#include "GL/glew.h"
#include <vector>

ModuleScene::ModuleScene()
{
	root = new GameObject(nullptr, "Root");
	mainCamera = new GameObject(root, "MainCamera");
	mainCamera->AddComponent(new Camera(nullptr));
}

bool ModuleScene::Init()
{
	GameObject* cullingCamera = new GameObject(root, "CullingCamera");
	cullingCamera->AddComponent(new Camera(nullptr));

	App->editorCamera->AddCamera(mainCamera->GetComponent<Camera>(), true);
	App->editorCamera->AddCamera(cullingCamera->GetComponent<Camera>());

	return true;
}

update_status ModuleScene::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_M))
	{
		LOG("Pres M, Save MainScene");
		char* buffer = nullptr;
		unsigned size = ImporterScene::Save(root, &buffer);
		App->filesystem->Save("MainScene", buffer, size);
	}
	else if (App->input->GetKey(SDL_SCANCODE_L))
	{
		LOG("Pres L, Load MainScene");
		char* buffer = nullptr;
		App->filesystem->basePath;
		App->filesystem->Load(App->filesystem->basePath.c_str(), "Library\\MainScene.meta", &buffer);
		delete(root);
		App->editor->SetSelectedGameObject(nullptr);
		ImporterScene::Load(buffer, root);
	}

	root->Update();
	return UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* newGameObject = new GameObject(parent ? parent : root, name);
	return newGameObject;
}

void ModuleScene::Draw(unsigned program)
{
	std::vector<GameObject*> childs = root->GetChilds();
	for each (GameObject* child in childs)
	{
		child->Draw(program);
	}
}


GameObject* ModuleScene::FindGameObjectId(unsigned int id)
{
	GameObject* go = nullptr;
	std::vector<GameObject*> childs = root->GetChilds();
	for each (GameObject * child in childs)
	{
		go = child->FindGameObjectId(id);
		if (go)
			return go;
	}

	return go;
}
