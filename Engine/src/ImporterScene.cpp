#include "ImporterScene.h"
#include "Application.h"
#include "ModuleModel.h"
#include "ModuleEditorCamera.h"
#include "Camera.h"
#include <map>

unsigned ImporterScene::Save(const GameObject* root, char** buffer)
{
	Config file;
	Config_Array goArray = file.SetArray("GameObjects");

	std::vector<const GameObject*> gameObjects;
	root->GetChilds(gameObjects);
	gameObjects.erase(gameObjects.begin());

	for (unsigned i = 0; i < gameObjects.size(); ++i)
	{
		SaveGameObject(goArray.AddNode(), gameObjects[i]);
	}

	unsigned size = file.Serialize(buffer);
	return size;
}

void ImporterScene::SaveGameObject(Config& config, const GameObject* gameObject)
{
	config.SetNumber("UID", gameObject->GetID());

	config.SetNumber("ParentUID", gameObject->parent ? gameObject->parent->GetID() : 0);
	config.SetString("Name", gameObject->name.c_str());

	config.SetBool("Active", gameObject->IsActive());

	Transform* transform = gameObject->GetComponent<Transform>();

	//Translation part
	config.SetArray("Translation").AddFloat3(transform->GetPosition());

	//Rotation part
	config.SetArray("Rotation").AddQuat(transform->GetRotation());

	//Scale part
	config.SetArray("Scale").AddFloat3(transform->GetScale());

	Config_Array compConfig = config.SetArray("Components");
	const std::vector<Component*> components = gameObject->GetAllComponents();

	for (unsigned i = 0; i < components.size(); i++)
	{
		if(components[i]->GetType() != Component::Type::Transform)
			SaveComponentBase(compConfig.AddNode(), components[i]);
	}
}

void ImporterScene::SaveComponentBase(Config& config, const Component* component)
{
	config.SetNumber("ComponentType", (int)component->GetType());
	//SaveComponent(config, component);

	if (component->GetType() == Component::Type::Mesh || component->GetType() == Component::Type::Material)
	{
		config.SetString("CustomPath", component->GetCustomPath().c_str());
	}
}

void ImporterScene::Load(const char* buffer, GameObject* &root)
{
	Config file(buffer);
	root = new GameObject();
	std::map<unsigned, GameObject*> createdGameObjects;
	Config_Array gameObjects_array = file.GetArray("GameObjects");

	for (unsigned i = 0; i < gameObjects_array.GetSize(); ++i)
	{
		//Single GameObject load
		Config gameObject_node = gameObjects_array.GetNode(i);

		float3 position = gameObject_node.GetArray("Translation").GetFloat3(0);
		Quat rotation = gameObject_node.GetArray("Rotation").GetQuat(0);
		float3 scale = gameObject_node.GetArray("Scale").GetFloat3(0);

		//Parent setup
		GameObject* parent = nullptr;
		std::map<unsigned, GameObject*>::iterator it = createdGameObjects.find(gameObject_node.GetNumber("ParentUID"));
		if (it != createdGameObjects.end())
			parent = it->second;

		GameObject* gameObject = new GameObject(parent ? parent : root, gameObject_node.GetString("Name").c_str(), position, rotation, scale);
		gameObject->SetID(gameObject_node.GetNumber("UID"));
		createdGameObjects[gameObject->GetID()] = gameObject;

		gameObject->SetActive(gameObject_node.GetBool("Active"));

		Config_Array components = gameObject_node.GetArray("Components");

		for (unsigned i = 0; i < components.GetSize(); i++)
		{
			Config comp = components.GetNode(i);
			Component::Type type = (Component::Type)((int)comp.GetNumber("ComponentType"));
						
			if (type == Component::Type::Mesh)
			{
				gameObject->AddComponent(App->model->LoadMesh(comp.GetString("CustomPath").c_str()));
				gameObject->GetComponent<Mesh>()->SetCustomPath(comp.GetString("CustomPath").c_str());
			}
			else if (type == Component::Type::Material)
			{
				gameObject->AddComponent(App->model->LoadMaterial(comp.GetString("CustomPath").c_str()));
				gameObject->GetComponent<Material>()->SetCustomPath(comp.GetString("CustomPath").c_str());
			}
			else if (type == Component::Type::Camera)
			{
				gameObject->AddComponent(new Camera(nullptr));
				App->editorCamera->AddCamera(gameObject->GetComponent<Camera>(), true);
			}
			//LoadComponent(comp, component);
		}

		//Call OnUpdateTransform() to init all components according to the GameObject
		gameObject->OnUpdateTransform();
	}
}