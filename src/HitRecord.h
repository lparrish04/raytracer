#pragma once
#include "Material.h"
#include "Shape.h"
#include <glm/glm.hpp>

class Shape;

struct HitRecord {
	Material mat;
	glm::vec3 N;  // normal
	glm::vec3 p;  // position
	Ray ray; // current ray
	Shape* shape; // hit shape
	float t; // t value
	float beta;
	float gamma;
};