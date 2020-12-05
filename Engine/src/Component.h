#pragma once

class GameObject;

class Component
{
public:
	enum Type
	{
		None,
		Transform,
		Mesh,
		Material
	};

public:
	Component(Type type, GameObject* gameObject);
	virtual ~Component();

	virtual void Update() {};

	void SetActive(bool isActive) { active = isActive; }
	bool IsActive() const { return active; };
	Type GetType() const { return type; };

public:
	GameObject* gameObject = nullptr;

protected:
	bool active = true;
	Type type = None;

};

