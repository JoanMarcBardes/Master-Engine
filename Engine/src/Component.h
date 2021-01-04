#pragma once
#include "Libraries/MathGeoLib/Math/float4x4.h"

class GameObject;

class Component
{
public:
	enum Type
	{
		None,
		Transform,
		Mesh,
		Material,
		Camera
	};

public:
	Component(Type type, GameObject* gameObject);
	virtual ~Component();

	virtual void Update() {};
	virtual void OnUpdateTransform(const float4x4& parent_global = float4x4::identity);

	void SetActive(bool isActive) { active = isActive; }
	bool IsActive() const { return active; };
	virtual Type GetType() { return type; };

public:
	GameObject* gameObject = nullptr;

protected:
	bool active = true;
	Type type = None;

};

