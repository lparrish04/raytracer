#include "Scene.h"

Scene::Scene(std::vector<std::unique_ptr<Shape>>& shapes,
	std::vector<std::unique_ptr<Light>>& lights) :
	shapes(shapes), lights(lights)
{
}

Scene::~Scene()
{
}

bool Scene::Hit(Ray ray, float tmin, float tmax, HitRecord& rec)
{
	float tcurrent = tmax;
	Shape* currentShape = nullptr;
	HitRecord closest;
	for (auto& shape : this->shapes)
	{
		HitRecord srec;

		if (shape.get()->intersect(ray, tmin, tmax, srec)) {
			if (srec.t < tcurrent) {
				tcurrent = srec.t;
				currentShape = srec.shape;
			}
		}
	}

	if (tcurrent < tmax)
	{
		rec.t = tcurrent;
		rec.shape = currentShape;
		rec.p = ray.o + rec.t * ray.dir;
		rec.N = rec.shape->normal(rec.p, closest);
		rec.mat = rec.shape->mat;
		rec.ray.o = rec.p;
		rec.ray.dir = glm::reflect(ray.dir, rec.N);

		return true;
	}

	return false;
}