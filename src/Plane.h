#pragma once
#include "Shape.h"

class Plane :
	public Shape
{
public:
	Plane(glm::vec3 p, glm::vec3 n, const Material& mat);
	~Plane();

	bool intersect(Ray ray, float tmin, float tmax, HitRecord& srec) override;
	glm::vec3 normal(glm::vec3 p, HitRecord& srec) override;
	bounds getBounds() override;

private:
	glm::vec3 p;
	glm::vec3 n;
	Material mat;
};
