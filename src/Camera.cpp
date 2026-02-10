#include "Camera.h"
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>


Camera::~Camera()
{
}

Camera::Camera(int widthRes,
	int heightRes,
	glm::vec3 eye,
	glm::vec3 lookat,
	glm::vec3 up,
	float fovy,
	float f)
{	
	// initialize members
	this->widthRes = widthRes;
	this->heightRes = heightRes;
	this->eye = eye;
	this->lookat = lookat;
	this->up = up;
	this->fovy = fovy;
	this->f = f;

	renderedImage = new float[widthRes * heightRes * 3];
}

void Camera::TakePicture(Scene *scene)
{
	/// compute camera vectors ///
	// camera front
	glm::vec3 w = glm::normalize(eye - lookat);
	// camera right
	glm::vec3 u = glm::normalize(glm::cross(up, w));
	// camera up
	glm::vec3 v = glm::cross(w, u);
	// image plane height in scene DO NEXT
	float planeHeight = 2 * f * tan(glm::radians(fovy / 2));
	float planeWidth = planeHeight * widthRes / heightRes;
	// pixel width and height
	float Pw = planeWidth / widthRes;
	float Ph = planeHeight / heightRes;
	// lower left corner
	glm::vec3 ll = (eye - w * f) - planeWidth * u / 2.0f - planeHeight * v / 2.0f;

	memset(renderedImage, 0, sizeof(float) * widthRes * heightRes * 3);

	// epsilon value
	const float eps = 0.0001f;

	// dummy rec var
	HitRecord dummy;

	// number of reflections to calcultae
	int numRays = 10;
	// levels of anti-aliasing
	int aaLevels = 0;
	// number of rays per pixel
	int pixelRayNum = glm::pow(4, aaLevels); // 1
	// number of rows/columns per pixel
	int subPixels = glm::sqrt(pixelRayNum); // 1

	for (int i = 0; i < widthRes; i++) {
		for (int j = 0; j < heightRes; j++) {

			glm::vec3 Pcol(0.0f);

			for (int ax = 0; ax < subPixels; ax++) { 
				for (int ay = 0; ay < subPixels; ay++) {

					// anti aliasing. 
					
					// pixel location in world
					glm::vec3 Pc = ll + (i + (ax + 0.5f) / subPixels) * Pw * u + (j + (ay + 0.5f) / subPixels) * Ph * v;
					Ray primary;
					primary.o = eye;
					primary.dir = glm::normalize(Pc - eye);
					HitRecord rec;
					HitRecord newRec;
					glm::vec3 c(0.0f);


					if (scene->Hit(primary, 0, INFINITY, rec)) // check primary ray. material properties are stored in rec.
					{
						//c = rec.mat.Ka;
						glm::vec3 reflectivity = rec.mat.Km;

						for (auto& light : scene->lights) // check against lights
						{
							Light* currentlight = light.get();

							glm::vec3 L = glm::normalize(currentlight->o - rec.p); // obj to light vector

							Ray lightray;
							lightray.o = rec.p; // hit position
							lightray.dir = L; // light vector

							if (!scene->Hit(lightray, eps, glm::length(currentlight->o - rec.p), dummy))
							{
								c += rec.mat.Kd * currentlight->col * glm::max(0.0f, glm::dot(rec.N, L)) +
									currentlight->col * rec.mat.Ks * glm::pow(glm::max(0.0f, glm::dot(glm::reflect(-L, rec.N), glm::normalize(primary.o - rec.p))), rec.mat.n); // phong reflection model
							}
						}

						// calculate reflected rays
						Ray ray = rec.ray; // reflected ray

						for (int bounces = 0; bounces < numRays; bounces++)
						{
							if (scene->Hit(ray, eps, INFINITY, newRec))
							{
								// shadow ray. newRec is now from hit position
								for (auto& light : scene->lights)
								{
									Light* currentlight = light.get();

									glm::vec3 L = glm::normalize(currentlight->o - newRec.p); // obj to light vector

									Ray lightray;
									lightray.o = newRec.p;
									lightray.dir = L;

									// TODO: separate reflectivity from diffuse to get bounce light.
									if (!scene->Hit(lightray, eps, glm::length(currentlight->o - newRec.p), dummy))
									{
										glm::vec3 reflectedRayColor = (newRec.mat.Kd * currentlight->col * glm::max(0.0f, glm::dot(newRec.N, L)) +
											currentlight->col * newRec.mat.Ks * glm::pow(glm::max(0.0f, glm::dot(glm::reflect(-L, newRec.N), glm::normalize(-ray.dir))), newRec.mat.n));

										c += reflectedRayColor * reflectivity;
										reflectivity *= newRec.mat.Km;
									}
								}
								reflectivity *= newRec.mat.Km;
								ray = newRec.ray;
							}
							else break;
						}
					}
					Pcol += c;
					
				}

			}
			Pcol = Pcol / (float)pixelRayNum;
			// average pixel and assign here
			int idx = (j * widthRes + i) * 3;
			renderedImage[idx + 0] = Pcol.x;
			renderedImage[idx + 1] = Pcol.y;
			renderedImage[idx + 2] = Pcol.z;
		}
	}
}
