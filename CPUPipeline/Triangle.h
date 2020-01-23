#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "FrameBuffer.h"
#include "Camera.h"
#include "../Figure.h"
#define _USE_MATH_DEFINES

#include <math.h>
class Figure;
class FrameBuffer;

class Triangle {
public:
	Triangle(unsigned int first, unsigned int second, unsigned int third, Figure* fig_);
	std::vector<unsigned int> vertices_ind;
	Figure* fig;
	std::vector<glm::vec4> after_transformations;
	std::vector<glm::vec4> before_last_transformations;
	std::vector<glm::vec4> w_points;
	std::vector<glm::vec4> normal_vectors;
	std::vector<glm::vec4> binormal_vectors;
	std::vector<glm::vec4> tangential_vectors;
	std::vector<glm::vec4> transformed_normal_vectors;
	std::vector<glm::vec4> transformed_tangential_vectors;
	std::vector<glm::vec4> transformed_binormal_vectors;

	glm::vec2 texture_cord[3] = { {0,0},{1,0},{1,1} };

	void DrawTriangle(bool backface_culling, bool paint_triangles, bool z_bufferng, bool perspective_correction, bool draw_edges, bool draw_color, FrameBuffer& fb, int color, Camera* cam);
	void DrawTriangleInternalWOClipping(bool backface_culling, bool paint_triangles, bool z_bufferng, bool perspective_correction, bool draw_edges, bool draw_color, FrameBuffer& fb, int color, Camera* cam);
	void DrawTriangleInternalClipping(bool backface_culling, bool paint_triangles, bool z_bufferng, bool perspective_correction, bool draw_edges, bool draw_color, FrameBuffer& fb, int color, Camera* cam);
	void CalculatePointsAfterTransformation();
	void CalculateNormalVectors();
	float CalculateSideValue();
};
