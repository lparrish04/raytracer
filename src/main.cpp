#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>
#include "Camera.h"
#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "BVH.h"
#include "Material.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 1200

float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
GLFWwindow *window;

// for .obj
std::vector<float> posBuff;
std::vector<float> texBuff;
std::vector<float> norBuff;
std::vector<glm::vec3> vertPos;
std::vector<glm::vec3> vertNor;


bool drawBunny = false;

// initialize shape and light vectors
std::vector<std::unique_ptr<Shape>> shapes;
std::vector<std::unique_ptr<Light>> lights;

std::vector<std::unique_ptr<Shape>> bunny;

std::vector<std::unique_ptr<Shape>> bvh;

Scene scene = Scene(shapes, lights);
Scene bvhScene = Scene(bvh, lights);

// materials
Material mat1;
Material mat2;
Material planeMat;
Material planeMat_red;
Material planeMat_green;
Material white;
Material mirrorMat;
Material bunnyMat;

// camera components
glm::vec3 eye(0.0f, 0.0f, 7.0f);
glm::vec3 lookat(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

void ClearFrameBuffer()
{
	memset(&frameBuffer, 0, WINDOW_HEIGHT * WINDOW_WIDTH * 3 * sizeof(float));
}

void LoadModel(const std::string& name)
{

	bunnyMat.Ka = glm::vec3(0.1f, 0.1f, 0.1f);
	bunnyMat.Kd = glm::vec3(0.0f, 0.0f, 1.0f);
	bunnyMat.Ks = glm::vec3(1.0f, 1.0f, 0.5f);
	bunnyMat.Km = glm::vec3(0.0f, 0.0f, 0.0f);
	bunnyMat.n = 100.0f;
	// Taken from Shinjiro Sueda with slight modification
	std::string meshName(name);
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if (!rc) {
		std::cerr << errStr << std::endl;
	}
	else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
					// add all x,y,z coords to vert vector
					vertPos.push_back(glm::vec3(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2]));
					if (!attrib.normals.empty()) {
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 0]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 1]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 2]);
						vertNor.push_back(glm::vec3(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]));
					}
					if (!attrib.texcoords.empty()) {
						texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
						texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);

					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
	// add verts to bunny shapes vector
	for (int i = 0; i < vertPos.size(); i += 3)
	{
		Tri currentTri;
		currentTri.p0 = glm::vec3(vertPos[i + 0]);
		currentTri.p1 = glm::vec3(vertPos[i + 1]);
		currentTri.p2 = glm::vec3(vertPos[i + 2]);
		currentTri.n0 = glm::vec3(vertNor[i + 0]);
		currentTri.n1 = glm::vec3(vertNor[i + 1]);
		currentTri.n2 = glm::vec3(vertNor[i + 2]);

		bunny.push_back(std::make_unique<Triangle>(currentTri, bunnyMat));
	}
}

void transform(std::vector<std::unique_ptr<Shape>>& shapes, glm::vec3 t, glm::vec3 s)
{
	for (auto& shape : shapes)
	{
		shape.get()->translate(t);
		shape.get()->scale(s);
	}
}

void Display()
{	
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, frameBuffer);
}

void TakePicture(Scene* sceneToDraw)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ClearFrameBuffer();

	Camera camera = Camera(WINDOW_WIDTH, WINDOW_HEIGHT, eye, lookat, up, 45.0f, 1.0f);

	auto start = std::chrono::high_resolution_clock::now();
	camera.TakePicture(sceneToDraw);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "Time passed (ms): " << duration.count() << std::endl;

	float* renderedImage = camera.GetRenderedImage();
	memcpy(frameBuffer, renderedImage, sizeof(float) * WINDOW_HEIGHT * WINDOW_WIDTH * 3);
}

void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	switch (key) {

	case 's':
		std::cout << "switching scene" << std::endl;
		drawBunny = !drawBunny;
		if (drawBunny) TakePicture(&bvhScene);
		else TakePicture(&scene);
		break;
	default:
		break;
	}
}

void createMaterials()
{
	// Set materials
	mat1.Ka = glm::vec3(0.1f, 0.1f, 0.1f);
	mat1.Kd = glm::vec3(0.2f, 1.0f, 0.2f);
	mat1.Ks = glm::vec3(1.0f, 1.0f, 1.0f);
	mat1.Km = glm::vec3(0.05f, 0.05f, 0.05f);
	mat1.n = 100.0f;

	mat2.Ka = glm::vec3(0.1f, 0.1f, 0.1f);
	mat2.Kd = glm::vec3(1.0f, 0.0f, 1.0f);
	mat2.Ks = glm::vec3(1.0f, 1.0f, 1.0f);
	mat2.Km = glm::vec3(0.0f, 0.0f, 0.0f);
	mat2.n = 10.0f;

	mirrorMat.Ka = glm::vec3(0.0f, 0.0f, 0.0f);
	mirrorMat.Kd = glm::vec3(0.0f, 0.0f, 0.0f);
	mirrorMat.Ks = glm::vec3(0.0f, 0.0f, 0.0f);
	mirrorMat.Km = glm::vec3(1.0f, 1.0f, 1.0f);
	mirrorMat.n = 0.0f;

	planeMat.Ka = glm::vec3(0.1f, 0.1f, 0.1f);
	planeMat.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
	planeMat.Ks = glm::vec3(0.0f, 0.0f, 0.0f);
	planeMat.Km = glm::vec3(0.0f, 0.0f, 0.0f);
	planeMat.n = 0.0f;

	planeMat_red.Ka = glm::vec3(0.1f, 0.1f, 0.1f);
	planeMat_red.Kd = glm::vec3(1.0f, 0.0f, 0.0f);
	planeMat_red.Ks = glm::vec3(0.0f, 0.0f, 0.0f);
	planeMat_red.Km = glm::vec3(0.0f, 0.0f, 0.0f);
	planeMat_red.n = 0.0f;

	planeMat_green.Ka = glm::vec3(0.1f, 0.1f, 0.1f);
	planeMat_green.Kd = glm::vec3(0.0f, 1.0f, 0.0f);
	planeMat_green.Ks = glm::vec3(0.0f, 0.0f, 0.0f);
	planeMat_green.Km = glm::vec3(0.0f, 0.0f, 0.0f);
	planeMat_green.n = 0.0f;
}

void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment 6 - Lane Parrish", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glfwSetCharCallback(window, CharacterCallback);

	// load bunny model
	LoadModel("../resources/bunny.obj");

	// transform bunny
	transform(bunny, glm::vec3(0.0f, -1.35f, 1.5f), glm::vec3(1.0f, 1.0f, 1.0f));

	createMaterials();

	// Add shapes
	shapes.push_back(std::make_unique<Sphere>(glm::vec3(-1.0f, -0.7f, 3.0f), 0.3f, mat1));
	shapes.push_back(std::make_unique<Sphere>(glm::vec3(1.5f, -0.5f, 3.0f), 0.5f, planeMat));
	shapes.push_back(std::make_unique<Sphere>(glm::vec3(-1.0f, 0.0f, 0.0f), 1.0f, mirrorMat));
	shapes.push_back(std::make_unique<Sphere>(glm::vec3(1.0f, 0.0f, -1.0f), 1.0f, mirrorMat));


	shapes.push_back(std::make_unique<Plane>(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), planeMat));
	shapes.push_back(std::make_unique<Plane>(glm::vec3(0.0f, 2.1f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), planeMat));
	shapes.push_back(std::make_unique<Plane>(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 1.0f), planeMat));
	shapes.push_back(std::make_unique<Plane>(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), planeMat_red));
	shapes.push_back(std::make_unique<Plane>(glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), planeMat_green));


	// Add lights
	lights.push_back(std::make_unique<Light>(glm::vec3(0.0f, 2.0f, -2.0f), glm::vec3(0.8, 0.8, 0.8)));
	lights.push_back(std::make_unique<Light>(glm::vec3(0.0f, 1.0f, 4.0f), glm::vec3(0.5, 0.5, 0.5)));

	bvh.push_back(std::make_unique<BVH>(bunny));

	shapes.insert(shapes.end(), std::make_move_iterator(bvh.begin()), std::make_move_iterator(bvh.end()));

	TakePicture(&scene);
}


int main()
{	
	Init();
	while ( glfwWindowShouldClose(window) == 0) 
	{
		glClear(GL_COLOR_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}