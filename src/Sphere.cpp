#include "Sphere.h"

Sphere::Sphere(glm::vec3 center, float r, const Material& mat) :
	Shape(mat), center(center), r(r)
{
	centroid = center;
}

Sphere::~Sphere()
{
}

bool Sphere::intersect(Ray ray, float tmin, float tmax, HitRecord& srec)
{
	glm::vec3 oc = ray.o - this->center; // center to ray origin
	float a = glm::dot(ray.dir, ray.dir);
	float b = glm::dot(2.0f * ray.dir, oc);
	float c = glm::dot(oc, oc) - (r * r);

	float disc = b * b - 4 * a * c; // discriminant

	// less than zero, no intersection
	if (disc < 0)
	{
		return false;
	}

	// greater than zero, 2 intersections. Use smaller t
	else
	{
		float t1 = (-b - glm::sqrt(disc)) / (2.0f * a);
		float t2 = (-b + glm::sqrt(disc)) / (2.0f * a);

		srec.mat = this->mat;
		float t = glm::min(t1, t2);

		if (t < tmin || t > tmax) return false;

		srec.t = t;
		srec.shape = this;

		return true;
	}
	return false;
}

glm::vec3 Sphere::normal(glm::vec3 p, HitRecord& srec)
{
	return glm::normalize(p - center);
}

bounds Sphere::getBounds()
{
	bounds sphereBound;

	sphereBound.bmax = center + r;
	sphereBound.bmin = center - r;

	return sphereBound;
}