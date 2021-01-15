#pragma once

#include <list>
#include <vector>
#include "Libraries/MathGeoLib/Geometry/AABB.h"
#include "GameObject.h"

#define QUADTREE_MIN_SIZE -30
#define QUADTREE_MAX_SIZE 30
#define MAX_ITEMS 2

class QuadtreeNode
{
public:
	QuadtreeNode(AABB box, QuadtreeNode* parent);
	~QuadtreeNode();

	void Insert(GameObject* go);
	void Remove(GameObject* go);

	template<typename TYPE>
	void Intersects(std::vector<GameObject*>& objects, const TYPE& primitive);

	void CreateChilds();
	void RedistributeChildren();

	bool IsLeaf();

private:
	math::AABB boundingbox;
	std::vector<GameObject*> objects;
	QuadtreeNode* parent = nullptr;
	QuadtreeNode* childs[4];
};

class QdTree
{
public:
	QdTree() {};
	~QdTree() {
		Clear();
	};

	void Create(math::AABB limits);
	void Clear();
	void Insert(GameObject* go);
	void Remove(GameObject* go);
	template<typename TYPE>
	inline void Intersects(std::vector<GameObject*>& objects, const TYPE& primitive);

public:
	float3 min_point;
	float3 max_point;
private:
	QuadtreeNode* root = nullptr;
};

template<typename TYPE>
inline void QdTree::Intersects(std::vector<GameObject*>& objects, const TYPE& primitive)
{
	if (root)
		root->Intersects(objects, primitive);
}

template<typename TYPE>
inline void QuadtreeNode::Intersects(std::vector<GameObject*>& objects, const TYPE& primitive)
{
	if (primitive.Intersects(boundingbox))
	{
		for (std::vector<GameObject*>::const_iterator it = this->objects.begin(); it != this->objects.end(); ++it)
		{
			if (std::find(objects.begin(), objects.end(), (*it)) == objects.end()) 
				if (primitive.Intersects((*it)->bounding_box)) {
					objects.push_back(*it);
				}

		}
		for (int i = 0; i < 4; ++i) {
			if (childs[i] != nullptr) childs[i]->Intersect(objects, primitive);
		}

	}
}