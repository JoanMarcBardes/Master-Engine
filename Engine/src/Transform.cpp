#include "Transform.h"
#include "Globals.h"
#include "GameObject.h"
#include "GL/glew.h"
#include "Libraries/ImGui/imgui.h"
#include "Libraries/ImGuizmo-master/ImGuizmo.h"

void MinScale(float3& scale)
{
	if (scale.x < 0.0001f) scale.x = 0.0001f;
	if (scale.y < 0.0001f) scale.y = 0.0001f;
	if (scale.z < 0.0001f) scale.z = 0.0001f;
}

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
	toUpdate = true;
	UpdateTransform();
}

void Transform::SetRotation(Quat newRotation)
{
	rotation = newRotation;
	toUpdate = true;
	UpdateTransform();
}

void Transform::SetScale(float3 newScale)
{
	MinScale(newScale);

	scale = newScale;
	toUpdate = true;
	UpdateTransform();
}

void Transform::SetRotationEuler(float3 newRotationEuler)
{
	float3 aux = (newRotationEuler - rotationEuler) * DEGTORAD;
	rotation = rotation * Quat::FromEulerXYZ(aux.x, aux.y, aux.z);
	rotationEuler = newRotationEuler;
	toUpdate = true;
	UpdateTransform();
}

void Transform::SetTransformGlobal(float4x4 newglobal)
{
	float3		newGposition;
	Quat		newGrotation;
	float3		newGscale;
	newglobal.Decompose(newGposition, newGrotation, newGscale);
	float3 newGrotationE = newGrotation.ToEulerXYZ() * RADTODEG;

	float3		gPosition;
	Quat		gRotation;
	float3		gScale;
	transformGlobal.Decompose(gPosition, gRotation, gScale);

	gPosition = gPosition - newGposition;

	float3 gRotationE = gRotation.ToEulerXYZ() * RADTODEG;
	float3 aux2 = (gRotationE - newGrotationE) * DEGTORAD;
	gRotation = Quat::FromEulerXYZ(aux2.x, aux2.y, aux2.z);

	gScale = float3(gScale.x / newGscale.x, gScale.y / newGscale.y, gScale.z / newGscale.z);
	MinScale(gScale);

	transform = float4x4::FromTRS(gPosition, gRotation, gScale);
	transformGlobal = newglobal;
	toUpdate = true;
}

void Transform::OnUpdateTransform(const float4x4& parentGlobal)
{
	transformGlobal = parentGlobal * transform;
	UpdatePosRotSca();
	toUpdate = false;
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

void Transform::Draw(unsigned program)
{
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&transformGlobal);
}