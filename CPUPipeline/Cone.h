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

class Cone : public Figure {
public:
	float height;
	int longitude;
	float r;

	Cone();
	void ModMenu(int counter, Figure*& to_delete) override;
	void CalculateVerticesAndTriangles() ;
	void CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri) override;

};