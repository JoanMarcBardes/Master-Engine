#include "ModuleScene.h"

ModuleScene::ModuleScene()
{
	root = new GameObject(nullptr, "Root");
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent)
{
	return new GameObject(parent ? parent : root, name);
}