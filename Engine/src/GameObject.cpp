#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "Globals.h"
#include "GL/glew.h"

LCG randomID;

GameObject::GameObject(const char* name) : name(name), parent(nullptr)
{
	id = randomID.Int();
	this->transform = new Transform(this, float3::zero, Quat::identity, float3::one);
	AddComponent(this->transform);
}

GameObject::GameObject(GameObject* parent, const float4x4& transform, const char* name) : parent(parent), name(name)
{
	id = randomID.Int();
	if (parent)
		parent->childs.push_back(this);
	this->transform = new Transform(this, transform);
	AddComponent(this->transform);
}

GameObject::GameObject(GameObject* parent, const char* name, const float3& translation, const Quat& rotation, const float3& scale) : parent(parent), name(name)
{
	id = randomID.Int();
	if (parent)
		parent->childs.push_back(this);
	this->transform = new Transform(this, translation, rotation, scale);
	AddComponent(this->transform);
}

GameObject::~GameObject()
{
	if (parent) {
		parent->RemoveChild(this);
		parent = nullptr;
	}

	for (int i = 0; i < childs.size(); i++)
	{
		delete childs[i];
	}
	childs.clear();

	for (int i = 0; i < components.size(); i++)
	{
		delete components[i];
	}
}


void GameObject::Update()
{
	if (transform->GetToUpdate())
		OnUpdateTransform();

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

	if(component->GetType() == Component::Type::Transform)
	{
		transform = (Transform*)component;
	}
}

Component* GameObject::GetComponent(Component::Type type)
{
	for each (Component* component in components)
	{
		if (component->GetType() == type)
			return component;
	}
	return nullptr;
}

/*template<typename AuxComponent>
AuxComponent* GameObject::GetComponent()
{
	Component::Type type = AuxComponent::GetType();
	for each (Component * component in components)
	{
		if (component->GetType() == type)
			return (AuxComponent*)component;
	}
	return nullptr;
}*/

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

GameObject* GameObject::FindGameObjectId(unsigned int target)
{
	GameObject* go = nullptr;
	if (id == target) return this;
	for each (GameObject * child in childs)
	{
		go = (*child).FindGameObjectId(target);
		if (go)
			return go;
	}
	return nullptr;
}

void GameObject::SetParent(GameObject* newParent)
{
	float4x4 global = float4x4::identity;
	if (newParent->parent == this)
	{
		LOG("[warning] make children the parent of the gameObject");
		newParent->SetParent(parent);
	}
	if (this != newParent && newParent != nullptr)
	{
		global = newParent->transform->GetTransformGlobal();

		if (parent != nullptr)
		{
			parent->RemoveChild(this);
		}
		parent = newParent;
		parent->childs.push_back(this);
	}
	transform->SetTransformGlobal(global);
}

void GameObject::RemoveChild(GameObject* gameObject)
{
	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); it++)
	{
		if ((*it) == gameObject)
		{
			childs.erase(it);
			break;
		}
	}
}


void GameObject::Draw(unsigned program)
{
	if (active)
	{		
		transform->Draw(program);

		Material* material = GetComponent< Material>();
		if (material)
			material->Draw(program);

		Mesh* mesh = GetComponent<Mesh>();
		if (mesh)
			mesh->Draw();

		
		// Draw the childs

		std::vector<GameObject*> childs = GetChilds();
		for each (GameObject* child in childs)
		{
			child->Draw(program);
		}
	}
	DrawBoundingBox();
}

void GameObject::DrawBoundingBox()
{
	glBegin(GL_LINES);
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

	for (int i = 0; i < bounding_box.NumEdges(); i++)
	{
		glVertex3f(bounding_box.Edge(i).a.x, bounding_box.Edge(i).a.y, bounding_box.Edge(i).a.z);
		glVertex3f(bounding_box.Edge(i).b.x, bounding_box.Edge(i).b.y, bounding_box.Edge(i).b.z);
	}
	glEnd();
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

}

void GameObject::OnUpdateTransform()
{
	float4x4 parentTransform = float4x4::identity;
	if(parent)
		parentTransform = parent->transform->GetTransformGlobal();
	transform->OnUpdateTransform(parentTransform);

	UpdateBoundingBox();

	for each (GameObject * child in childs)
	{
		child->OnUpdateTransform();
	}
}

void GameObject::UpdateBoundingBox() {

	bounding_box.SetNegativeInfinity();

	Mesh* mesh = (Mesh*)GetComponent(Component::Type::Mesh);

	if (mesh) {
		bounding_box.Enclose(mesh->GetMin(), mesh->GetMax());
	}

	if (transform && mesh) {

		obb.SetFrom(bounding_box);
		obb.Transform(transform->GetTransformGlobal());
		if (obb.IsFinite()) {
			bounding_box = obb.MinimalEnclosingAABB();
		}
	}

	for (int i = 0; i < childs.size(); ++i) {

		childs[i]->UpdateBoundingBox();
	}

}
