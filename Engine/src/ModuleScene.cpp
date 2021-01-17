#include "ModuleScene.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEditorCamera.h"
#include "ModuleInput.h"
#include "ImporterScene.h"
#include "ModuleFilesystem.h"
#include "ModuleEditor.h"
#include "ModuleModel.h"
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
	Camera* cullCam = (Camera*)cullingCamera->GetComponent(Component::Type::Camera);
	cullCam->cullingCam = true;

	/*App->editorCamera->AddCamera(mainCamera->GetComponent<Camera>(), true);
	App->editorCamera->AddCamera(cullingCamera->GetComponent<Camera>());*/

	BuildQuadtree();
	return true;
}

update_status ModuleScene::Update()
{
	root->Update();
	return UPDATE_CONTINUE;
}

void ModuleScene::Save()
{
	char* buffer = nullptr;
	unsigned size = ImporterScene::Save(root, &buffer);
	App->filesystem->Save("MainScene", buffer, size);
	LOG("Scene Saved");
}

void ModuleScene::Load()
{
	char* buffer = nullptr;
	App->filesystem->basePath;
	unsigned size = App->filesystem->Load("", "Library\\MainScene.meta", &buffer);
	delete(root);
	gameObjects.clear();
	App->editorCamera->RemoveAllCameras();
	App->editor->SetSelectedGameObject(nullptr);
	App->model->Clean();
	ImporterScene::Load(buffer, root);
	gameObjects.push_back(root);

	if (gameObjects[0]->GetChilds().size() > 0) {
		AddGameObjects(root);
	}

	BuildQuadtree();
	LOG("Scene Loaded");
}


GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* newGameObject = new GameObject(parent ? parent : root, name);
	newGameObject->UpdateBoundingBox();
	gameObjects.push_back(newGameObject);
	return newGameObject;
}

void ModuleScene::AddGameObjects(GameObject* go) {
	std::vector<GameObject*> childs = go->GetChilds();

	for (int i = 0; i < childs.size(); ++i) {
		gameObjects.push_back(childs[i]);
		if (childs[i]->GetChilds().size() > 0) {
			AddGameObjects(childs[i]);
		}
	}
}

void ModuleScene::BuildQuadtree()
{
	//If there's quadtree, delete it
	if (quadtree != nullptr) {
		RELEASE(quadtree);
	}

	//Calculate quadtree size
	float3 min_point(QUADTREE_MIN_SIZE); //Minimum quadtree size
	float3 max_point(QUADTREE_MAX_SIZE); //Maximum quadtree size
	CalculateQuadtreeSize(min_point, max_point);

	//Create quadtree
	quadtree = new QdTree();
	AABB root_quad_node = AABB(min_point, max_point);
	quadtree->Create(root_quad_node);

	//Insert all the GameObjects to the quadtree
	for (int i = 0; i < gameObjects.size(); ++i) {
		gameObjects[i]->UpdateBoundingBox();
		quadtree->Insert(gameObjects[i]);
	}

}

void ModuleScene::CalculateQuadtreeSize(float3& min_point, float3& max_point)
{
	for (int i = 0; i < gameObjects.size(); ++i) {
		//Min point
		float3 min_p = gameObjects[i]->bounding_box.minPoint;
		if (min_p.x < min_point.x)
			min_point.x = min_p.x;
		if (min_p.y < min_point.y)
			min_point.y = min_p.y;
		if (min_p.z < min_point.z)
			min_point.z = min_p.z;

		//Max point
		float3 max_p = gameObjects[i]->bounding_box.maxPoint;
		if (max_p.x > max_point.x)
			max_point.x = max_p.x;
		if (max_p.y > max_point.y)
			max_point.y = max_p.y;
		if (max_p.z > max_point.z)
			max_point.z = max_p.z;
	}
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
