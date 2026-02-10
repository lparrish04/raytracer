#pragma once
#include "Shape.h"
#include "BVH.h"
#include <vector>

struct Tri {
	glm::vec3 p0;
	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 n0;
	glm::vec3 n1;
	glm::vec3 n2;
};

class Triangle :
	public Shape
{
public:
	Triangle(Tri tri, const Material& mat);
	~Triangle();

	bool intersect(Ray ray, float tmin, float tmax, HitRecord& srec) override;
	glm::vec3 normal(glm::vec3 p, HitRecord& srec) override;
	bounds getBounds() override;

	void translate(glm::vec3 t) override;
	void scale(glm::vec3 s) override;

	float beta;
	float gamma;
	glm::vec3 E1;
	glm::vec3 E2;

	Tri tri;
	Material mat;

private:
};
