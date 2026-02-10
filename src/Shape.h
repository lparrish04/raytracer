#pragma once
#include <glm/glm.hpp>
#include "Ray.h"
#include "Material.h"
#include "HitRecord.h"

struct bounds {
	glm::vec3 bmax;
	glm::vec3 bmin;
};

class Shape
{
public:
	Shape(const Material& mat) : mat(mat) {}
	~Shape(void);

	virtual bool intersect(Ray ray, float tmin, float tmax, HitRecord& srec) = 0;
	virtual glm::vec3 normal(glm::vec3 p, HitRecord& srec) = 0;
	// TODO: add bounding box calculation function
	virtual bounds getBounds() = 0;
	virtual void translate(glm::vec3 t) {}
	virtual void scale(glm::vec3 s) {}

	Material mat;
	glm::vec3 centroid;

private:

};
