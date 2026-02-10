#include "BVH.h"
#include "Triangle.h"
#include <iostream>
#include <algorithm>
#include <numeric>


BVH::BVH(const std::vector<std::unique_ptr<Shape>>& shapes) : Shape(mat), shapes(shapes)
{
	std::cout << "starting BVH construction\n";
	bbmin = glm::vec3(INFINITY);
	bbmax = glm::vec3(-INFINITY);
	// get full bounding box
	for (auto& shape : shapes)
	{
		bbmin = glm::min(bbmin, shape.get()->getBounds().bmin);
		bbmax = glm::max(bbmax, shape.get()->getBounds().bmax);
	}
	// TODO: build node tree
	nodes.resize(shapes.size() * 2 - 1);
	// create root node. Contains all shapes.
	BVHnode& root = nodes[0];
	root.shapeIndices.resize(shapes.size());
	std::iota(root.shapeIndices.begin(), root.shapeIndices.end(), 0);
	nextNode = 1;
	updateBounds(0);
	splitNode(0, 0);
	std::cout << "BVH construction finished with " << numNodes << " nodes\n";
}

BVH::~BVH()
{
}

void BVH::splitNode(int nodeIdx, int axis)
{
	int leafSize = 3; // Number of shapes to hold in the final leaf node
	numNodes++;

	BVHnode& currentNode = nodes[nodeIdx];
	int nodeSize = currentNode.shapeIndices.size();

	if (nodeSize <= 3) return;  // dont subdivide if the node has three or less shapes

	std::vector<float> centroids(nodeSize);
	std::vector<int> order(nodeSize);
	std::iota(order.begin(), order.end(), 0);

	axis = (axis + 1) % 3;

	// go through all shapes in node, add centroid of selected axis to centroids vector
	for (int i = 0; i < nodeSize; i++)
	{
		int shapeIndex = currentNode.shapeIndices[i];
		centroids[i] = shapes[shapeIndex].get()->centroid[axis];
	}

	// sort order vector based on centroid vector values.
	std::sort(order.begin(), order.end(),
		[&](int a, int b) {
			return centroids[a] < centroids[b];
		});

	for (int i = 0; i < nodeSize; i++) {
		order[i] = currentNode.shapeIndices[order[i]];
	}
	
	int midpoint = nodeSize / 2;

	// split sorted order into left and right children
	std::vector<int> left(order.begin(), order.begin() + midpoint);
	std::vector<int> right(order.begin() + midpoint, order.end());

	

	// assign left and right children indices
	int leftIndex = nextNode++;
	int rightIndex = nextNode++;
	currentNode.lChild = leftIndex;
	currentNode.rChild = rightIndex;

	// assign sorted indices to children, update bounds
	BVHnode leftChildNode;
	leftChildNode.shapeIndices = left;
	leftChildNode.aabbmin = glm::vec3(INFINITY);
	leftChildNode.aabbmax = glm::vec3(-INFINITY);
	nodes[leftIndex] = leftChildNode;
	updateBounds(leftIndex);

	BVHnode rightChildNode;
	rightChildNode.shapeIndices = right;
	rightChildNode.aabbmin = glm::vec3(INFINITY);
	rightChildNode.aabbmax = glm::vec3(-INFINITY);
	nodes[rightIndex] = rightChildNode;
	updateBounds(rightIndex);

	if (leftChildNode.shapeIndices.size() > leafSize) 
		splitNode(leftIndex, axis);

	if (rightChildNode.shapeIndices.size() > leafSize)
		splitNode(rightIndex, axis);
	
}

void BVH::updateBounds(int nodeIdx)
{
	BVHnode& currentNode = nodes[nodeIdx];
	// loop through all shapes in current node to compute bounds
	for (int i = 0; i < currentNode.shapeIndices.size(); i++)
	{
		Shape* currentShape = shapes[currentNode.shapeIndices[i]].get();
		currentNode.aabbmin = glm::min(currentNode.aabbmin, currentShape->getBounds().bmin);
		currentNode.aabbmax = glm::max(currentNode.aabbmax, currentShape->getBounds().bmax);
	}
}

bool boxIntersection(const Ray ray, glm::vec3 bmin, glm::vec3 bmax, float tmin, float tmax)
{
	// two intersection points on slab
	float tx1 = (bmin.x - ray.o.x) / ray.dir.x;
	float tx2 = (bmax.x - ray.o.x) / ray.dir.x;
	// get min and max of intersections to find entry/exit points
	float txmin = glm::min(tx1, tx2);
	float txmax = glm::max(tx1, tx2);

	// repeat for y & z
	float ty1 = (bmin.y - ray.o.y) / ray.dir.y;
	float ty2 = (bmax.y - ray.o.y) / ray.dir.y;
	float tymin = glm::min(ty1, ty2);
	float tymax = glm::max(ty1, ty2);

	float tz1 = (bmin.z - ray.o.z) / ray.dir.z;
	float tz2 = (bmax.z - ray.o.z) / ray.dir.z;
	float tzmin = glm::min(tz1, tz2);
	float tzmax = glm::max(tz1, tz2);

	// find min and max of all sides.
	float tbmin = glm::max(txmin, glm::max(tymin, tzmin));
	float tbmax = glm::min(txmax, glm::min(tymax, tzmax));

	if (tbmax < tbmin) return false;  // No intersection
	if (tbmax < tmin) return false;    // Box is behind ray start
	if (tbmin > tmax) return false;   // Box is beyond current hit

	return true;
}

void BVH::traverseNodes(int nodeIdx, const Ray& ray, float tmin, float& tcurrent, HitRecord& srec, bool& hit)
{
	BVHnode& node = nodes[nodeIdx];

	if (boxIntersection(ray, node.aabbmin, node.aabbmax, tmin, tcurrent))
	{
		// if node has no children, it is a leaf node. check all shapes
		if (node.lChild == -1 && node.rChild == -1)
		{
			for (int idx : node.shapeIndices)
			{
				if (shapes[idx].get()->intersect(ray, tmin, tcurrent, srec)) {
					if (srec.t < tcurrent)
					{
						tcurrent = srec.t;
						hit = true;
					}
				}
			}
		}
		else
		{
			if (node.lChild != -1) traverseNodes(node.lChild, ray, tmin, tcurrent, srec, hit);
			if (node.rChild != -1) traverseNodes(node.rChild, ray, tmin, tcurrent, srec, hit);
		}
	}
}

bool BVH::intersect(Ray ray, float tmin, float tmax, HitRecord& srec)
{

	if (boxIntersection(ray, bbmin, bbmax, tmin, tmax))
	{
		// Traverse node tree
		bool hit = false;
		traverseNodes(0, ray, tmin, tmax, srec, hit);

		return hit;
	}
	return false;
}

glm::vec3 BVH::normal(glm::vec3 p, HitRecord& srec)
{
	return glm::vec3(0.0f);
}

bounds BVH::getBounds()
{
	bounds BVHbounds;
	return BVHbounds;
}

