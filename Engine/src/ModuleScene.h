#pragma once
#include "Module.h"
#include "GameObject.h"
#include "QuadTree.h"
#include <vector>

class ModuleScene : public Module
{
public:
	ModuleScene();
	
	bool Init() override;
	update_status Update()override;


	GameObject* CreateGameObject(const char* name, GameObject* parent = nullptr);

	GameObject* GetRoot() { return root; }

	void BuildQuadtree();
	void CalculateQuadtreeSize(float3& min_point, float3& max_point);
	void Draw(unsigned program);

	GameObject* FindGameObjectId(unsigned int id);

public:
	QdTree* quadtree = nullptr;

private:
	std::vector<GameObject*> gameObjects;
	GameObject* root;
	GameObject* mainCamera;
};
