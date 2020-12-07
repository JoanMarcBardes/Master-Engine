#pragma once
#include "Module.h"
#include "GameObject.h"
#include <vector>

class ModuleScene : public Module
{
public:
	ModuleScene();
	
	bool Init() override;
	// TODO
	/*
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;*/

	GameObject* CreateGameObject(const char* name, GameObject* parent = nullptr);

	GameObject* GetRoot() { return root; }

	void Draw(unsigned program);

private:
	std::vector<GameObject*> gameObjects;
	GameObject* root;

	unsigned idVertex = 0;
	unsigned idFragment = 0;
};
