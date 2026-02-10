#pragma once
#include <glm/glm.hpp>

class Light
{
public:
	Light(glm::vec3 o, glm::vec3 col); // origin, color, intensity
	~Light();

	glm::vec3 o;
	glm::vec3 col;

private:
};
