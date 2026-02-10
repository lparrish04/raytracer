#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Scene.h"



class Camera
{
public:
	Camera(int widthRes,
		int heightRes,
		glm::vec3 eye,
		glm::vec3 lookat,
		glm::vec3 up,
		float fovy,
		float f);

	~Camera();

	void TakePicture(Scene *scene);
	float* GetRenderedImage() { return renderedImage; };

private:

	int widthRes;
	int heightRes;
	glm::vec3 eye;
	glm::vec3 lookat;
	glm::vec3 up;
	float fovy;
	float f;

	float *renderedImage;

};
