#pragma once
#include <glm/glm.hpp>


struct Material {
	glm::vec3 Ka; // ambient
	glm::vec3 Kd; // diffuse
	glm::vec3 Ks; // specular
	glm::vec3 Km; // reflectivity
	glm::vec3 Kr; // refractivity
	float n;
};