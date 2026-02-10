#pragma once
#include "Shape.h"


class Sphere :
	public Shape
{
public:
	Sphere(glm::vec3 center, float r, const Material& mat);
	~Sphere();

	bool intersect(Ray ray, float tmin, float tmax, HitRecord& srec) override;
	glm::vec3 normal(glm::vec3 p, HitRecord& srec) override;
	bounds getBounds() override;

private:
	glm::vec3 center;
	float r; // radius
	Material mat;
};
