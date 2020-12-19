#pragma once
#include "Component.h"
#include "Libraries/MathGeoLib/MathGeoLib.h"


class GameObject;

class Transform : public Component
{
public:
	Transform(GameObject* ownerGameObject, const float4x4& transform);
	Transform(GameObject* ownerGameObject, float3 position = float3::zero, Quat rotation = Quat::identity, float3 scale = float3::one);

	//get
	float4x4 GetTransform() const		{ return transform; }
	float3	 GetPosition() const		{ return position; }
	Quat	 GetRotation() const		{ return rotation; }
	float3	 GetScale() const			{ return scale; }
	float3	 GetRotationEuler() const	{ return rotationEuler; }
	float4x4 GetTransformGlobal() const { return transformGlobal; }
	bool GetToUpdate() const			{ return toUpdate; }
	static Type GetType()				{ return Type::Transform; };

	//set
	void SetTransform(float4x4 transform);
	void SetPosition(float3 position);
	void SetRotation(Quat rotation);
	void SetScale(float3 scale);
	void SetRotationEuler(float3 rotation_euler);
	void SetTransformGlobal(float4x4 transform_global);

	 void OnUpdateTransform(const float4x4& parent_global = float4x4::identity) override;

	void Draw(unsigned program);

private:
	void UpdateTransform();
	void UpdatePosRotSca();

private:
	float4x4	transform;
	float3		position;
	Quat		rotation;
	float3		scale;
	float3		rotationEuler;

	float4x4	transformGlobal = float4x4::identity;
	bool toUpdate = false;

};

