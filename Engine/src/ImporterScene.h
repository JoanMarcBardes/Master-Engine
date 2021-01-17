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

	//Process a Component base data into a config file
	//This function will call specific functions for each component type
	static void SaveComponentBase(Config& config, const Component* component);

	//Select the specific component class to be saved and calls its according function
	//void SaveComponent(Config& config, const Component* component);

	//Process a Camera component data into a config file
	//void SaveComponent(Config& config, const C_Camera* component);

	//Loads the base for all components and calls the specific component load function
	//void LoadComponent(Config& config, Component* component);

	//void LoadComponent(Config& config, C_Camera* camera);
};

