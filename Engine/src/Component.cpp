#include "Component.h"

Component::Component(Type type, GameObject* gameObject) : type(type), gameObject(gameObject)
{
}

Component::~Component()
{

}

void Component::OnUpdateTransform(const float4x4& parent_global)
{
	//for override
}