#pragma once
#include "Shape.h"
#include <vector>
#include <memory>

class BVH :
	public Shape
{
public:
	BVH(const std::vector<std::unique_ptr<Shape>>& shapes);
	~BVH();

	bool intersect(const Ray ray, float tmin, float tmax, HitRecord& srec) override;
	glm::vec3 normal(glm::vec3 p, HitRecord& srec) override;
	bounds getBounds() override;


	void splitNode(int nodeIxd, int axis);
	void updateBounds(int nodeIdx);
	void traverseNodes(int nodeIdx, const Ray& ray, float tmin, float& tcurrent, HitRecord& srec, bool& hit);

	const std::vector<std::unique_ptr<Shape>>& shapes;
	

private:
	struct BVHnode {
		glm::vec3 aabbmin, aabbmax;
		int lChild = -1, rChild = -1; // index of left and right child
		std::vector<int> shapeIndices;
		/// Note that only the indices of each shape from the original shape vector will be stored. 
	};

	std::vector<BVHnode> nodes;

	int nextNode;
	int numNodes = 0;

	glm::vec3 bbmin = glm::vec3(0.0f);
	glm::vec3 bbmax = glm::vec3(0.0f);
	
	Material mat;
};
