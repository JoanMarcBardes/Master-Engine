#include "GameObject.h"
#include "Globals.h"

GameObject::GameObject(const char* name) : name(name)
{
	AddComponent(new Transform(this, float3::zero, Quat::identity, float3::one));
}

GameObject::GameObject(GameObject* parent, const float4x4& transform, const char* name) : parent(parent), name(name)
{
	if (parent)
		parent->childs.push_back(this);
	AddComponent(new Transform(this, transform));
}

GameObject::GameObject(GameObject* parent, const char* name, const float3& translation, const Quat& rotation, const float3& scale) : parent(parent), name(name)
{
	if (parent)
		parent->childs.push_back(this);
	AddComponent(new Transform(this, translation, rotation, scale));
}


void GameObject::Update()
{
	std::vector<Component*>::iterator it;
	for (it = components.begin(); it != components.end(); ++it)
	{
		(*it)->Update();
	}

	for each (GameObject* child in childs)
	{
		if (child->active)
			child->Update();
	}
}

Component* GameObject::CreateComponent(Component::Type type)
{
	if (ContainsType(type))
	{
		LOG("[error] this GameObject already constais this type: %s", type);
		return nullptr;
	}
	Component* newComponent = new Component(type,this);
	components.push_back(newComponent);

	return newComponent;
}

void GameObject::AddComponent(Component* component)
{
	if (ContainsType(component->GetType()))
	{
		LOG("[error] this GameObject already constais this type: %s", component->GetType());
		return;
	}
	components.push_back(component);
	component->gameObject = this;
}

bool GameObject::ContainsType(Component::Type type)
{
	for each (Component* component in components)
	{
		if (component->GetType() == type)
			return true;
	}
	return false;
}

std::vector<GameObject*>& GameObject::GetChilds()
{
	/*std::vector<GameObject*> vector;
	for each (GameObject* child in childs)
	{
		vector.push_back(child);
	}
	return vector;*/
	return childs;
}

GameObject* GameObject::GetChild(const char* name) const
{
	for each (GameObject* child in childs)
	{
		if (child->name == name)
			return child;
	}
	LOG("[error] don't find any GameObject with name: %s", name);
	return nullptr;
}
