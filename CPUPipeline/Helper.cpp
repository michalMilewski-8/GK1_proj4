#include "../Helper.h"

glm::mat4 Helper::createRotationMatrix(float x_angle, float y_angle, float z_angle, Figure* fig) {
	glm::mat4 x_rotate = glm::mat4(1.0f);
	x_rotate[1][1] = std::cos(x_angle);
	x_rotate[2][1] = -std::sin(x_angle);
	x_rotate[1][2] = std::sin(x_angle);
	x_rotate[2][2] = std::cos(x_angle);

	glm::mat4 y_rotate = glm::mat4(1.0f);
	y_rotate[0][0] = std::cos(y_angle);
	y_rotate[2][0] = -std::sin(y_angle);
	y_rotate[0][2] = std::sin(y_angle);
	y_rotate[2][2] = std::cos(y_angle);

	glm::mat4 z_rotate = glm::mat4(1.0f);
	z_rotate[0][0] = std::cos(z_angle);
	z_rotate[1][0] = -std::sin(z_angle);
	z_rotate[0][1] = std::sin(z_angle);
	z_rotate[1][1] = std::cos(z_angle);
	fig->x_angle = x_angle;
	fig->y_angle = y_angle;
	fig->z_angle = z_angle;

	return x_rotate * y_rotate * z_rotate;
}

void Helper::getRotateValue(float& x, float& y, float& z, Figure* fig) {
	x = fig->x_angle;
	y = fig->y_angle;
	z = fig->z_angle;
}

glm::mat4 Helper::createTranslationMatrix(float x, float y, float z) {
	glm::mat4 translate = glm::mat4(1.0f);
	translate[3][0] = x;
	translate[3][1] = y;
	translate[3][2] = z;

	return translate;
}

void Helper::getTranslationValues(float& x, float& y, float& z, glm::mat4& translate) {
	x = translate[3][0];
	y = translate[3][1];
	z = translate[3][2];
}

glm::mat4 Helper::createScaleMatrix(float x, float y, float z) {
	glm::mat4 scale = glm::mat4(1.0f);
	scale[0][0] = x;
	scale[1][1] = y;
	scale[2][2] = z;

	return scale;
}

void Helper::getScaleValue(float& x, float& y, float& z, glm::mat4& scale) {
	x = scale[0][0];
	y = scale[1][1];
	z = scale[2][2];
}

float Helper::Length(glm::vec3 vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

float Helper::Length(glm::vec4 vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}

float Helper::Length(glm::vec2 vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y);

}
