#include "Transform.h"
#include "Globals.h"
#include "GameObject.h"

Transform::Transform(GameObject* ownerGameObject, const float4x4& transform) : 
	Component(Component::Type::Transform, ownerGameObject), transform(transform)
{
	transform.Decompose(position, rotation, scale);
	rotationEuler = rotation.ToEulerXYZ() * RADTODEG;
}

Transform::Transform(GameObject* ownerGameObject, float3 position, Quat rotation, float3 scale) : 
	Component(Component::Type::Transform, ownerGameObject), position(position), rotation(rotation), scale(scale)
{
	transform = float4x4::FromTRS(position, rotation, scale);
	rotationEuler = rotation.ToEulerXYZ() * RADTODEG;
}

void Transform::SetTransform(float4x4 newTransform)
{
	transform = newTransform;
	transform.Decompose(position, rotation, scale);
}

void Transform::SetPosition(float3 newPosition)
{
	position = newPosition;
	UpdateTransform();
}

void Transform::SetRotation(Quat newRotation)
{
	rotation = newRotation;
	UpdateTransform();
}

void Transform::SetScale(float3 newScale)
{
	scale = newScale;
	UpdateTransform();
}

void Transform::SetRotationEuler(float3 newRotationEuler)
{
	float3 aux = (newRotationEuler - rotationEuler) * DEGTORAD;
	rotation = rotation * Quat::FromEulerXYZ(aux.x, aux.y, aux.z);
	rotationEuler = newRotationEuler;
	UpdateTransform();
}


void Transform::SetTransformGlobal(float4x4 global)
{
	Transform* parentTransform = (Transform*)gameObject->parent->GetComponent(Component::Type::Transform);
	float4x4 localTransform = parentTransform->GetTransformGlobal().Inverted() * global;
	transform = localTransform;
	transformGlobal = global;
}


void Transform::OnUpdateTransform(const float4x4& parent_global)
{
	transformGlobal = parent_global * transform;
	UpdatePosRotSca();
}

void Transform::UpdateTransform()
{
	transform = float4x4::FromTRS(position, rotation, scale);
}

void Transform::UpdatePosRotSca()
{
	transform.Decompose(position, rotation, scale);
	rotationEuler = rotation.ToEulerXYZ() * RADTODEG;
}