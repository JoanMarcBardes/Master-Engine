#include "Quadtree.h"
#include "GameObject.h"
#include "Mesh.h"


QuadtreeNode::QuadtreeNode(AABB bbox, QuadtreeNode* parent) : boundingbox(bbox), parent(parent)
{
	for (int i = 0; i < 4; ++i) {
		childs[i] = nullptr;
	}
}

QuadtreeNode::~QuadtreeNode()
{
	for (int i = 0; i < 4; ++i) {
		delete childs[i];
		childs[i] = nullptr;
	}
}

void QuadtreeNode::Insert(GameObject* go)
{
	if (IsLeaf()) {
		if (go->bounding_box.Intersects(boundingbox)) {
			if (objects.size() < MAX_ITEMS) {
				objects.push_back(go);

			}
			else {
				CreateChilds();
				objects.push_back(go);

				unsigned int boxes_intersection = 0;
				for (int i = 0; i < 4; ++i) {
					if (childs[i]->boundingbox.Intersects(go->bounding_box)) {
						boxes_intersection++;
					}
				}

				if (boxes_intersection > 1) {
					for (int i = 0; i < 4; ++i) {
						delete childs[i];
						childs[i] = nullptr;
					}
				}else
					RedistributeChildren();
			}
		}
	}
	else {
		for (int i = 0; i < 4; ++i)
			childs[i]->Insert(go);
	}
}

void QuadtreeNode::Remove(GameObject* go)
{
	for (std::vector<GameObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it)
		if ((*it) == go) {
			objects.erase(it);
			break;
		}

	if (!IsLeaf()) {
		for (int i = 0; i < 4; ++i)
			childs[i]->Remove(go);
	}
}

void QuadtreeNode::CreateChilds()
{
	AABB child_box;
	float3 box_center = boundingbox.CenterPoint();
	float3 child_size = { boundingbox.Size().x / 2.0f, boundingbox.Size().y, boundingbox.Size().z / 2.0f };

	float3 child_center = { box_center.x - boundingbox.Size().x / 4.0f, box_center.y, box_center.z + boundingbox.Size().z / 4.0f };
	child_box.SetFromCenterAndSize(child_center, child_size);
	childs[0] = new QuadtreeNode(child_box, this); //top Left

	child_center = { box_center.x + boundingbox.Size().x / 4.0f, box_center.y, box_center.z + boundingbox.Size().z / 4.0f };
	child_box.SetFromCenterAndSize(child_center, child_size);
	childs[1] = new QuadtreeNode(child_box, this); //Top Right

	child_center = { box_center.x - boundingbox.Size().x / 4.0f, box_center.y, box_center.z - boundingbox.Size().z / 4.0f };
	child_box.SetFromCenterAndSize(child_center, child_size);
	childs[2] = new QuadtreeNode(child_box, this); //Bot Left

	child_center = { box_center.x + boundingbox.Size().x / 4.0f, box_center.y, box_center.z - boundingbox.Size().z / 4.0f };
	child_box.SetFromCenterAndSize(child_center, child_size);
	childs[3] = new QuadtreeNode(child_box, this); //Bot Right
}

void QuadtreeNode::RedistributeChildren()
{
	std::vector<GameObject*>::const_iterator it = objects.begin();

	while (it != objects.end()) {

		GameObject* go = *it;

		unsigned int boxes_intersection = 0;
		bool intersects[4];
		for (int i = 0; i < 4; ++i) {
			intersects[i] = childs[i]-> boundingbox.Intersects((*it)->bounding_box);
			if (intersects[i])
				boxes_intersection++;
		}

		if (boxes_intersection > 1) {
			it++;
		}
		else {
			it = objects.erase(it);
			for (int i = 0; i < 4; ++i)
				if (intersects[i])
					childs[i]->objects.push_back(go);
		}
	}
}

bool QuadtreeNode::IsLeaf()
{
	if (childs[0] == nullptr)
		return true;
	else
		return false;
}

void QdTree::Create(math::AABB limits)
{
	Clear();
	root = new QuadtreeNode(limits, nullptr);

	min_point = limits.minPoint;
	max_point = limits.maxPoint;

}

void QdTree::Clear()
{
	delete root;
	root = nullptr;
}

void QdTree::Insert(GameObject* go) // enclose
{
	if (root)
		root->Insert(go);
}

void QdTree::Remove(GameObject* go)
{
	if (root)
		root->Remove(go);
}
