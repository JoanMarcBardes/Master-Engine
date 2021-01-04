#pragma once
#include "Module.h"
#include "GameObject.h"
#include <vector>

class ModuleScene : public Module
{
public:
	ModuleScene();
	
	bool Init() override;
	update_status Update()override;


	GameObject* CreateGameObject(const char* name, GameObject* parent = nullptr);

	GameObject* GetRoot() { return root; }

	void Draw(unsigned program);

	GameObject* FindGameObjectId(unsigned int id);

private:
	std::vector<GameObject*> gameObjects;
	GameObject* root;
	GameObject* mainCamera;
};
