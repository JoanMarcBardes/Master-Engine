#include "ModuleScene.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEditorCamera.h"
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
	root->Update();
	return UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* newGameObject = new GameObject(parent ? parent : root, name);
	newGameObject->UpdateBoundingBox();
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
