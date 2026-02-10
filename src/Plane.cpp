#include "Plane.h"


Plane::Plane(glm::vec3 p, glm::vec3 n, const Material& mat) : Shape(mat), p(p), n(n)
{
	centroid = p;
}

Plane::~Plane()
{
}

bool Plane::intersect(Ray ray, float tmin, float tmax, HitRecord& srec)
{
	glm::vec3 po = this->p - ray.o; // ray origin to plane point

	float t = glm::dot(po, this->n) / glm::dot(ray.dir, this->n);

	if (t < tmin || t > tmax) return false;

	srec.t = t;
	srec.shape = this;

	return true;
}

glm::vec3 Plane::normal(glm::vec3 p, HitRecord& srec)
{
	return this->n;
}

bounds Plane::getBounds()
{
	bounds planeBounds;

	planeBounds.bmin = glm::vec3(-INFINITY);
	planeBounds.bmax = glm::vec3(INFINITY);

	return planeBounds;
}

