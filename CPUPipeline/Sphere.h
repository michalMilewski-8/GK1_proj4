#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "FrameBuffer.h"
#include "Camera.h"
#define _USE_MATH_DEFINES

#include <math.h>
#include "Figure.h"


class Sphere : public Figure {
public:
	int lattitude;
	int longitude;
	float r;

	Sphere();
	void CalculateVerticesAndTriangles();
	void CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri);

};