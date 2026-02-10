#pragma once
#include <vector>
#include <memory>
#include "Shape.h"
#include "Light.h"
#include "Ray.h"
#include "HitRecord.h"


class Scene
{
public:
	Scene(std::vector<std::unique_ptr<Shape>>& shapes,
		std::vector<std::unique_ptr<Light>>& lights);
	~Scene();

	bool Hit(Ray ray, float tmin, float tmax, HitRecord& rec);

	std::vector<std::unique_ptr<Shape>>& shapes;
	std::vector<std::unique_ptr<Light>>& lights;

private:

	

};
