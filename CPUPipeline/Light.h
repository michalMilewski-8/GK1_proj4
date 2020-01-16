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

class Light {
	public:
		glm::vec3 position = { 0,0,2 };
		glm::vec3 Id = {1,1,1};
		glm::vec3 Is = { 1,1,1 };
		float Ac = 1;
		float Al = 0.09f;
		float Aq = 0.032f;

		float CalculateAttenuatiom(glm::vec3 to);
		glm::vec3 CalculateValueLightVal(glm::vec3 to,glm::vec3 N, glm::vec3 kd, float ks, int n, glm::vec3 cam_pos);
};