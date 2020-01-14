#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "../Figure.h"
#define _USE_MATH_DEFINES

#include <math.h>
class Figure; 

static class Helper {
public:
	static glm::mat4 createRotationMatrix(float x_angle, float y_angle, float z_angle, Figure* fig);
	static void getRotateValue(float& x, float& y, float& z, Figure* fig);
	static glm::mat4 createTranslationMatrix(float x, float y, float z);
	static void getTranslationValues(float& x, float& y, float& z, glm::mat4& translate);
	static glm::mat4 createScaleMatrix(float x, float y, float z);
	static void getScaleValue(float& x, float& y, float& z, glm::mat4& scale);

};