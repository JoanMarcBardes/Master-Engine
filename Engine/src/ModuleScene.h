#pragma once
#include "Module.h"
#include "GameObject.h"
#include <vector>

class ModuleScene : public Module
{
public:
	ModuleScene();
	
	// TODO
	/*bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;*/

	GameObject* CreateGameObject(const char* name, GameObject* parent = nullptr);

private:
	std::vector<GameObject*> gameObjects;
	GameObject* root;
};
