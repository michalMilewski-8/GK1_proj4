#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "FrameBuffer.h"
#include "Camera.h"
#define _USE_MATH_DEFINES

#include <math.h>

class Figure;

enum Type {
	Cube_,
	Sphere_,
	Cone_,
	Cylinder_
};

class Triangle {
public:
	Triangle(unsigned int first, unsigned int second, unsigned int third, Figure* fig_);
	std::vector<unsigned int> vertices_ind;
	Figure* fig;
	std::vector<glm::vec4> after_transformations;
	std::vector<glm::vec4> normal_vectors;
	std::vector<glm::vec4> binormal_vectors;
	std::vector<glm::vec4> tangential_vectors;
	std::vector<glm::vec4> transformed_normal_vectors;
	std::vector<glm::vec4> transformed_tangential_vectors;
	std::vector<glm::vec4> transformed_binormal_vectors;

	void DrawTriangle(bool backface_culling,bool paint_triangles, bool z_bufferng, bool perspective_correction, FrameBuffer& fb,int color, Camera* cam);
	void CalculatePointsAfterTransformation();
	void CalculateNormalVectors();
	float CalculateSideValue();
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
	glm::mat4 modelT_1 = glm::mat4(1.0f);

	float x_angle = 0.0f;
	float y_angle = 0.0f;
	float z_angle = 0.0f;

	virtual void CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri) = 0;

	void Transform(glm::mat4 proj, glm::mat4 view);
};

class Cube : public Figure {
public:
	float edges_lengts[12];

	Cube(std::vector<glm::vec4> vert);
	Cube();
	void CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri);
	void CalculateEdgesLengths();
	void ChangeEdgeLength(int edge, float new_length);
};

class Sphere : public Figure {
public:
	int lattitude;
	int longitude;
	float r;

	Sphere();
	void CalculateVerticesAndTriangles();
	void CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri);

};

class Cylinder : public Figure {
public:
	float height;
	int longitude;
	float r;

	Cylinder();
	void CalculateVerticesAndTriangles();
	void CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri);

};

class Cone : public Figure {
public:
	float height;
	int longitude;
	float r;

	Cone();
	void CalculateVerticesAndTriangles();
	void CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri);

};