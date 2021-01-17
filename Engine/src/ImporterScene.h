#pragma once
#include "GameObject.h"
#include "Component.h"
#include "Config.h"

class ImporterScene
{
public:
	static unsigned Save(const GameObject* root, char** buffer);
	static void Load(const char* buffer, GameObject* &root);

private:
	static void SaveGameObject(Config& config, const GameObject* gameObject);
	static void SaveComponentBase(Config& config, const Component* component);

	//TODO
	//void SaveComponent(Config& config, const C_Camera* component);
	//void LoadComponent(Config& config, Component* component);
	//void LoadComponent(Config& config, C_Camera* camera);
};

