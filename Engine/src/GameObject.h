#pragma once
#include "Component.h"
#include "Libraries/MathGeoLib/MathGeoLib.h"
#include "Transform.h"
#include <string>
#include <vector>

class Transform;

class GameObject
{
public:
	GameObject(const char* name = "newGameObject");
	GameObject(GameObject* parent, const float4x4& transform = float4x4::identity, const char* name = "newGameObject");
	GameObject(GameObject* parent, const char* name = "newGameObject", const float3& translation = float3::zero, const Quat& rotation = Quat::identity, const float3& scale = float3::one);
	~GameObject();

	void Update();
	Component* CreateComponent(Component::Type type);
	void AddComponent(Component* component);
	Component* GetComponent(Component::Type type);
	template<typename AuxComponent>
	AuxComponent* GetComponent()
	{
		Component::Type type = AuxComponent::GetType();
		for each (Component * component in components)
		{
			if (component->GetType() == type)
				return (AuxComponent*)component;
		}
		return nullptr;
	}

	void SetActive(bool isActive) { active = isActive; }
	bool IsActive() const { return active; };
	unsigned GetID() const { return id; }

	std::vector<GameObject*>& GameObject::GetChilds();
	GameObject* GetChild(const char* name) const;
	unsigned GetNumChilds() const { return childs.size(); }
	GameObject* FindGameObjectId(unsigned int id);

	void SetParent(GameObject* gameObject);
	void RemoveChild(GameObject* gameObject);

	void Draw(unsigned program);

	void SetProgram(unsigned newProgram) { program = newProgram; }
	unsigned GetProgram() { return program; }

	void OnUpdateTransform();

private:
	bool ContainsType(Component::Type type); // GameObject only can contains one component of the same type

public:
	std::string name;
	GameObject* parent = nullptr;

private:
	unsigned program = 0;
	unsigned id = 0;

	bool active = true;
	std::vector<GameObject*> childs;

	std::vector<Component*> components;
	Transform* transform = nullptr; // this component it's mandatory
};

