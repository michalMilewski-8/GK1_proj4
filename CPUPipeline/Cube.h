#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "FrameBuffer.h"
#include "Camera.h"
#define _USE_MATH_DEFINES

#include <math.h>
#include "../Figure.h"
#include "../Triangle.h"

class Cube : public Figure {
public:
	float edges_lengts[12];

	Cube(std::vector<glm::vec4> vert);
	Cube();
	void ModMenu(int counter, Figure*& to_delete) override;
	void CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri) override;
	void CalculateEdgesLengths();
	void ChangeEdgeLength(int edge, float new_length);
};