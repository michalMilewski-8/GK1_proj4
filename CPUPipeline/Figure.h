#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "FrameBuffer.h"
#include "Camera.h"
#include "../Triangle.h"
#define _USE_MATH_DEFINES

#include <math.h>

class Triangle;
enum Type {
	Cube_,
	Sphere_,
	Cone_,
	Cylinder_
};

class Figure {
public:
	std::vector<Triangle> triangles;
	std::vector<glm::vec4> vertices;
	Type type;

	glm::mat4 rotate = glm::mat4(1.0f);
	glm::mat4 scale = glm::mat4(1.0f);
	glm::mat4 translate = glm::mat4(1.0f);
	glm::mat4 center = glm::mat4(1.0f);

	glm::mat4 transformation = glm::mat4(1.0f);

	glm::mat4 trans_without_viewport = glm::mat4(1.0f);

	float x_angle = 0.0f;
	float y_angle = 0.0f;
	float z_angle = 0.0f;


	virtual void ModMenu(int counter, Figure*& to_delete);
	virtual void CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri) = 0;
	void Transform(glm::mat4 proj, glm::mat4 view,glm::mat4 view_port);
};
