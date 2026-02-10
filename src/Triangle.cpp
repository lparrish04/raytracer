#include "Triangle.h"


Triangle::Triangle(Tri tri, const Material& mat) : Shape(mat), tri(tri)
{
	E1 = tri.p1 - tri.p0;
	E2 = tri.p2 - tri.p0;
	centroid = (tri.p0 + tri.p1 + tri.p2) / 3.0f;
}

Triangle::~Triangle()
{
}

bool Triangle::intersect(Ray ray, float tmin, float tmax, HitRecord& srec)
{
	glm::vec3 S = ray.o - tri.p0;
	glm::vec3 S1 = glm::cross(ray.dir, E2);
	glm::vec3 S2 = glm::cross(S, E1);

	float det = glm::dot(S1, E1);

	if (det < 0.0001f) return false; // ray is parallel or backface

	float invDet = 1.0f / det;

	this->beta = glm::dot(S, S1) * invDet;
	if (this->beta < 0 || this->beta > 1) return false;

	this->gamma = glm::dot(ray.dir, S2) * invDet;
	if (this->gamma < 0 || this->gamma + this->beta > 1) return false; // outside of triangle

	float t = glm::dot(S2, E2) * invDet;
	if (t < tmin || t > tmax) return false; // outside of t bounds

	srec.t = t;
	srec.shape = this;

	return true;
}

glm::vec3 Triangle::normal(glm::vec3 p, HitRecord& srec)
{
	return glm::normalize((1 - this->beta - this->gamma) * tri.n0 + this->beta * tri.n1 + this->gamma * tri.n2);
}

bounds Triangle::getBounds()
{
	bounds triBounds;

	triBounds.bmin = glm::min(this->tri.p0, glm::min(this->tri.p1, this->tri.p2));
	triBounds.bmax = glm::max(this->tri.p0, glm::max(this->tri.p1, this->tri.p2));

	return triBounds;
}

void Triangle::translate(glm::vec3 t)
{
	tri.p0 += t;
	tri.p1 += t;
	tri.p2 += t;
}

void Triangle::scale(glm::vec3 s)
{
	tri.p0 *= s;
	tri.p1 *= s;
	tri.p2 *= s;
}

