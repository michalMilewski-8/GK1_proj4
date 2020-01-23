#include "../Triangle.h"

void Triangle::CalculateNormalVectors() {

	fig->CalculateNormalVectors(vertices_ind[0], vertices_ind[1], vertices_ind[2], this);
}

float Triangle::CalculateSideValue() {
	auto one = after_transformations[0];
	auto two = after_transformations[1];
	auto three = after_transformations[2];


	glm::vec3 one_ = (two - one);
	glm::vec3 two_ = (three - one);
	if ((one_.x <= 0.01f && one_.x >= -0.01f && one_.y <= 0.01f && one_.y >= -0.01f && one_.z <= 0.01f && one_.z >= -0.01f) || (two_.x <= 0.01f && two_.x >= -0.01f && two_.y <= 0.01f && two_.y >= -0.01f && two_.z <= 0.01f && two_.z >= -0.01f))
		return -1;
	auto tmp = glm::cross(two_, one_);
	return glm::dot(glm::vec3(tmp.x, tmp.y, tmp.z), glm::vec3({ 0, 0, -1 }));
}

Triangle::Triangle(unsigned int first, unsigned int second, unsigned int third, Figure* fig_)
{
	vertices_ind = { first,second,third };
	fig = fig_;
	CalculateNormalVectors();
}

float CalculateDist(glm::vec4 v, int i) {
	switch (i) {
	case 1: return v.w + v.x;
		break;
	case 2: return v.w - v.x;
		break;
	case 3: return v.w + v.y;
		break;
	case 4: return v.w - v.y;
		break;
	case 5: return v.w + v.z;
		break;
	case 6: return v.w - v.z;
		break;
	default:
		return 1;
		break;
	}
}

glm::vec3 AproxNormLin(int x, int y, int x0, int y0, glm::vec3 N0, int x1, int y1, glm::vec3 N1) {
	float len = std::sqrt((x0 - x) * (x0 - x) + (y0 - y) * (y0 - y)) / std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
	return N0 * (1 - len) + N1 * len;
}

glm::vec3 AproxNormValue2(int x0, int y0, glm::vec3 N0, int x1, int y1, glm::vec3 N1, int x2, int y2, glm::vec3 N2, int x, int y) {
	float div = ((y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2));
	if (div != 0) {
		float w0 = ((y1 - y2) * (x - x2) + (x2 - x1) * (y - y2)) / div;
		float w1 = ((y2 - y0) * (x - x2) + (x0 - x2) * (y - y2)) / div;
		float w2 = 1 - w0 - w1;
		return w0 * N0 + w1 * N1 + w2 * N2;
	}
	return N1;
}

bool collinear(int x1, int y1, int x2, int y2, int x3, int y3) {
	return (y1 - y2) * (x1 - x3) == (y1 - y3) * (x1 - x2);
}

void Triangle::DrawTriangle(bool backface_culling, bool paint_triangles, bool z_bufferng, bool perspective_correction, bool draw_edges, bool draw_color, FrameBuffer& fb, int color, Camera* cam)
{
	float val = 1.0f;
	if (backface_culling)
		val = CalculateSideValue();
	if (val >= 0) {
		bool clipping = false;
		for (int i = 0; i < 3; i++) {
			if (after_transformations[i].x<0 ||
				after_transformations[i].x>fb.m_width ||
				after_transformations[i].y < 0 ||
				after_transformations[i].y > fb.m_height ||
				CalculateDist(before_last_transformations[i], 5) < 0 ||
				CalculateDist(before_last_transformations[i], 6) < 0) {
				clipping = true;
				break;
			}
		}
		if (clipping)
			DrawTriangleInternalClipping(backface_culling, paint_triangles, z_bufferng, perspective_correction, draw_edges, draw_color, fb, color, cam);
		else
			DrawTriangleInternalWOClipping(backface_culling, paint_triangles, z_bufferng, perspective_correction, draw_edges, draw_color, fb, color, cam);
	}
}

void Triangle::DrawTriangleInternalWOClipping(bool backface_culling, bool paint_triangles, bool z_bufferng, bool perspective_correction, bool draw_edges, bool draw_color, FrameBuffer& fb, int color, Camera* cam)
{

	glm::vec3 N0, N1, N2;
	glm::vec3 B0, B1, B2;
	glm::vec3 T0, T1, T2;

	auto tmp = after_transformations[0];
	auto tmp2 = after_transformations[1];
	auto tmp3 = after_transformations[2];

	N0 = transformed_normal_vectors[0];
	N1 = transformed_normal_vectors[1];
	N2 = transformed_normal_vectors[2];

	TriangleParams params;

	for (int i = 0; i < 3; i++) {
		params.A[i] = before_last_transformations[i];
		params.B[i] = transformed_binormal_vectors[i];
		params.T[i] = transformed_tangential_vectors[i];
		params.N[i] = transformed_normal_vectors[i];
	}


	if (paint_triangles)
	{
		if (z_bufferng)
			if (draw_color)
				if (fig->draw_normal_texture)
					fb.FillTriangle(tmp.x, tmp.y, tmp.z, tmp2.x, tmp2.y, tmp2.z, tmp3.x, tmp3.y, tmp3.z, params, fig, cam, this);
				else
					fb.FillTriangle(tmp.x, tmp.y, tmp.z, tmp2.x, tmp2.y, tmp2.z, tmp3.x, tmp3.y, tmp3.z, N0, N1, N2, w_points[0], w_points[1], w_points[2], fig, cam, this);
			else
				fb.FillTriangle(tmp.x, tmp.y, tmp.z, tmp2.x, tmp2.y, tmp2.z, tmp3.x, tmp3.y, tmp3.z, color);
		else
			fb.FillTriangle(tmp.x, tmp.y, tmp2.x, tmp2.y, tmp3.x, tmp3.y, color);

	}
	if (draw_edges)
		if (z_bufferng) {
			fb.DrawLine(tmp.x, tmp.y, tmp.z, tmp2.x, tmp2.y, tmp2.z, RGB(255, 0, 0));
			fb.DrawLine(tmp2.x, tmp2.y, tmp2.z, tmp3.x, tmp3.y, tmp3.z, RGB(255, 0, 0));
			fb.DrawLine(tmp3.x, tmp3.y, tmp3.z, tmp.x, tmp.y, tmp.z, RGB(255, 0, 0));
		}
		else
		{
			fb.DrawLine(tmp.x, tmp.y, tmp2.x, tmp2.y, RGB(255, 0, 0));
			fb.DrawLine(tmp2.x, tmp2.y, tmp3.x, tmp3.y, RGB(255, 0, 0));
			fb.DrawLine(tmp3.x, tmp3.y, tmp.x, tmp.y, RGB(255, 0, 0));
		}


}


void Triangle::DrawTriangleInternalClipping(bool backface_culling, bool paint_triangles, bool z_bufferng, bool perspective_correction, bool draw_edges, bool draw_color, FrameBuffer& fb, int color, Camera* cam)
{

	std::vector<glm::vec4> points_to;
	std::vector<glm::vec4> points;
	bool add = true;
	for (int i = 0; i < 3; i++) {
		glm::vec4 A = before_last_transformations[i % 3];
		glm::vec4 B = before_last_transformations[(i + 1) % 3];
		for (int j = 1; j <= 6; j++) {
			add = true;
			float da = CalculateDist(A, j);
			float db = CalculateDist(B, j);
			if (da < 0 && db < 0) {
				add = false;
				break;
			}
			float dc = da / (da - db);
			if (da < 0) {
				A = A * (1 - dc) + B * dc;
			}
			if (db < 0) {
				B = A * (1 - dc) + B * dc;
			}
		}
		if (add) {

			points_to.push_back(A);
			points_to.push_back(B);
		}
	}

	points_to.erase(std::unique(points_to.begin(), points_to.end()), points_to.end());
	if (points_to.size() != 0 && *(points_to.end() - 1) == *points_to.begin()) {
		points_to.erase(points_to.end() - 1, points_to.end());
	}
	TriangleParams params;

	if (points_to.size() > 0) {
		auto D = points_to[0];
		points = points_to;
		//points.push_back(before_last_transformations[0]);
		//points.push_back(before_last_transformations[1]);
		//points.push_back(before_last_transformations[2]);

		for (int k = 1; k < ((int)points.size() - 1); k++) {
			auto tmp_ = points[0] / points[0].w;
			auto tmp2_ = points[k] / points[k].w;
			auto tmp3_ = points[k + 1] / points[k + 1].w;

			glm::vec3 N0, N1, N2;
			glm::vec3 B0, B1, B2;
			glm::vec3 T0, T1, T2;

			auto tmp = cam->GetViewPortMatrix() * tmp_;
			auto tmp2 = cam->GetViewPortMatrix() * tmp2_;
			auto tmp3 = cam->GetViewPortMatrix() * tmp3_;

			if (fig->draw_normal_texture) {
				bool is0 = false;
				bool is1 = false;
				bool is2 = false;
				if (tmp == after_transformations[0]) {
					N0 = transformed_normal_vectors[0];
					B0 = transformed_binormal_vectors[0];
					T0 = transformed_tangential_vectors[0];
					is0 = true;
				}
				else if (tmp == after_transformations[1]) {
					B0 = transformed_binormal_vectors[1];
					T0 = transformed_tangential_vectors[1];
					N0 = transformed_normal_vectors[1];
					is0 = true;
				}
				else if (tmp == after_transformations[2]) {
					B0 = transformed_binormal_vectors[2];
					T0 = transformed_tangential_vectors[2];
					N0 = transformed_normal_vectors[2];
					is0 = true;
				}
				if (tmp2 == after_transformations[0]) {
					B1 = transformed_binormal_vectors[0];
					T1 = transformed_tangential_vectors[0];
					N1 = transformed_normal_vectors[0];
					is1 = true;
				}
				else if (tmp2 == after_transformations[1]) {
					B1 = transformed_binormal_vectors[1];
					T1 = transformed_tangential_vectors[1];
					N1 = transformed_normal_vectors[1];
					is1 = true;
				}
				else if (tmp2 == after_transformations[2]) {
					B1 = transformed_binormal_vectors[2];
					T1 = transformed_tangential_vectors[2];
					N1 = transformed_normal_vectors[2];
					is1 = true;
				}
				if (tmp3 == after_transformations[0]) {
					B2 = transformed_binormal_vectors[0];
					T2 = transformed_tangential_vectors[0];
					N2 = transformed_normal_vectors[0];
					is2 = true;
				}
				else if (tmp3 == after_transformations[1]) {
					B2 = transformed_binormal_vectors[1];
					T2 = transformed_tangential_vectors[1];
					N2 = transformed_normal_vectors[1];
					is2 = true;
				}
				else if (tmp3 == after_transformations[2]) {
					B2 = transformed_binormal_vectors[2];
					T2 = transformed_tangential_vectors[2];
					N2 = transformed_normal_vectors[2];
					is2 = true;
				}
				if (!is0)
				{
					N0 = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, transformed_normal_vectors[0], after_transformations[1].x, after_transformations[1].y, transformed_normal_vectors[1], after_transformations[2].x, after_transformations[2].y, transformed_normal_vectors[2], tmp.x, tmp.y);
					B0 = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, transformed_binormal_vectors[0], after_transformations[1].x, after_transformations[1].y, transformed_binormal_vectors[1], after_transformations[2].x, after_transformations[2].y, transformed_binormal_vectors[2], tmp.x, tmp.y);
					T0 = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, transformed_tangential_vectors[0], after_transformations[1].x, after_transformations[1].y, transformed_tangential_vectors[1], after_transformations[2].x, after_transformations[2].y, transformed_tangential_vectors[2], tmp.x, tmp.y);
				}
				if (!is1)
				{
					N1 = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, transformed_normal_vectors[0], after_transformations[1].x, after_transformations[1].y, transformed_normal_vectors[1], after_transformations[2].x, after_transformations[2].y, transformed_normal_vectors[2], tmp2.x, tmp2.y);;
					B1 = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, transformed_binormal_vectors[0], after_transformations[1].x, after_transformations[1].y, transformed_binormal_vectors[1], after_transformations[2].x, after_transformations[2].y, transformed_binormal_vectors[2], tmp2.x, tmp2.y);;
					T1 = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, transformed_tangential_vectors[0], after_transformations[1].x, after_transformations[1].y, transformed_tangential_vectors[1], after_transformations[2].x, after_transformations[2].y, transformed_tangential_vectors[2], tmp2.x, tmp2.y);;
				}
				if (!is2) 
				{
					N2 = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, transformed_normal_vectors[0], after_transformations[1].x, after_transformations[1].y, transformed_normal_vectors[1], after_transformations[2].x, after_transformations[2].y, transformed_normal_vectors[2], tmp3.x, tmp3.y);
					B2 = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, transformed_binormal_vectors[0], after_transformations[1].x, after_transformations[1].y, transformed_binormal_vectors[1], after_transformations[2].x, after_transformations[2].y, transformed_binormal_vectors[2], tmp3.x, tmp3.y);
					T2 = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, transformed_tangential_vectors[0], after_transformations[1].x, after_transformations[1].y, transformed_tangential_vectors[1], after_transformations[2].x, after_transformations[2].y, transformed_tangential_vectors[2], tmp3.x, tmp3.y);
				}

				params.N[0] = N0;
				params.N[1] = N1;
				params.N[2] = N2;
				params.B[0] = B0;
				params.B[1] = B1;
				params.B[2] = B2;
				params.T[0] = T0;
				params.T[1] = T1;
				params.T[2] = T2;
			
			}
			else {
				bool is0 = false;
				bool is1 = false;
				bool is2 = false;
				if (tmp == after_transformations[0]) {
					N0 = transformed_normal_vectors[0];
					is0 = true;
				}
				else if (tmp == after_transformations[1]) {
					N0 = transformed_normal_vectors[1];
					is0 = true;
				}
				else if (tmp == after_transformations[2]) {
					N0 = transformed_normal_vectors[2];
					is0 = true;
				}
				if (tmp2 == after_transformations[0]) {
					N1 = transformed_normal_vectors[0];
					is1 = true;
				}
				else if (tmp2 == after_transformations[1]) {
					N1 = transformed_normal_vectors[1];
					is1 = true;
				}
				else if (tmp2 == after_transformations[2]) {
					N1 = transformed_normal_vectors[2];
					is1 = true;
				}
				if (tmp3 == after_transformations[0]) {
					N2 = transformed_normal_vectors[0];
					is2 = true;
				}
				else if (tmp3 == after_transformations[1]) {
					N2 = transformed_normal_vectors[1];
					is2 = true;
				}
				else if (tmp3 == after_transformations[2]) {
					N2 = transformed_normal_vectors[2];
					is2 = true;
				}
				if (!is0)

					N0 = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, transformed_normal_vectors[0], after_transformations[1].x, after_transformations[1].y, transformed_normal_vectors[1], after_transformations[2].x, after_transformations[2].y, transformed_normal_vectors[2], tmp.x, tmp.y);
				if (!is1)
					N1 = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, transformed_normal_vectors[0], after_transformations[1].x, after_transformations[1].y, transformed_normal_vectors[1], after_transformations[2].x, after_transformations[2].y, transformed_normal_vectors[2], tmp2.x, tmp2.y);;
				if (!is2)
					N2 = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, transformed_normal_vectors[0], after_transformations[1].x, after_transformations[1].y, transformed_normal_vectors[1], after_transformations[2].x, after_transformations[2].y, transformed_normal_vectors[2], tmp3.x, tmp3.y);
			}
			params.A[0] = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, w_points[0], after_transformations[1].x, after_transformations[1].y, w_points[1], after_transformations[2].x, after_transformations[2].y, w_points[2], tmp.x, tmp.y);
			params.A[1] = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, w_points[0], after_transformations[1].x, after_transformations[1].y, w_points[1], after_transformations[2].x, after_transformations[2].y, w_points[2], tmp2.x, tmp2.y);
			params.A[2] = AproxNormValue2(after_transformations[0].x, after_transformations[0].y, w_points[0], after_transformations[1].x, after_transformations[1].y, w_points[1], after_transformations[2].x, after_transformations[2].y, w_points[2], tmp3.x, tmp3.y);

			if (paint_triangles)
			{
				if (z_bufferng)
					if (draw_color)
						if (fig->draw_normal_texture)
							fb.FillTriangle(tmp.x, tmp.y, tmp.z, tmp2.x, tmp2.y, tmp2.z, tmp3.x, tmp3.y, tmp3.z, params, fig, cam, this);
						else
							fb.FillTriangle(tmp.x, tmp.y, tmp.z, tmp2.x, tmp2.y, tmp2.z, tmp3.x, tmp3.y, tmp3.z, N0, N1, N2, params.A[0], params.A[1], params.A[2], fig, cam, this);
					else
						fb.FillTriangle(tmp.x, tmp.y, tmp.z, tmp2.x, tmp2.y, tmp2.z, tmp3.x, tmp3.y, tmp3.z, color);
				else
					fb.FillTriangle(tmp.x, tmp.y, tmp2.x, tmp2.y, tmp3.x, tmp3.y, color);

			}
			if (draw_edges)
				if (z_bufferng) {
					fb.DrawLine(tmp.x, tmp.y, tmp.z, tmp2.x, tmp2.y, tmp2.z, RGB(255, 0, 0));
					fb.DrawLine(tmp2.x, tmp2.y, tmp2.z, tmp3.x, tmp3.y, tmp3.z, RGB(255, 0, 0));
					fb.DrawLine(tmp3.x, tmp3.y, tmp3.z, tmp.x, tmp.y, tmp.z, RGB(255, 0, 0));
				}
				else
				{
					fb.DrawLine(tmp.x, tmp.y, tmp2.x, tmp2.y, RGB(255, 0, 0));
					fb.DrawLine(tmp2.x, tmp2.y, tmp3.x, tmp3.y, RGB(255, 0, 0));
					fb.DrawLine(tmp3.x, tmp3.y, tmp.x, tmp.y, RGB(255, 0, 0));
				}
		}
	}

}

void Triangle::CalculatePointsAfterTransformation() {
	after_transformations = { fig->transformation * fig->vertices[vertices_ind[0]],
 fig->transformation * fig->vertices[vertices_ind[1]],
fig->transformation * fig->vertices[vertices_ind[2]] };

	before_last_transformations = { fig->trans_without_viewport * fig->vertices[vertices_ind[0]],
 fig->trans_without_viewport * fig->vertices[vertices_ind[1]],
fig->trans_without_viewport * fig->vertices[vertices_ind[2]] };

	w_points = { fig->model_matrix * fig->vertices[vertices_ind[0]],
 fig->model_matrix * fig->vertices[vertices_ind[1]],
fig->model_matrix * fig->vertices[vertices_ind[2]] };


	transformed_normal_vectors = { fig->re_model_matrix * normal_vectors[0],
 fig->re_model_matrix * normal_vectors[1],
fig->re_model_matrix * normal_vectors[2] };

	transformed_binormal_vectors = { fig->re_model_matrix * binormal_vectors[0],
 fig->re_model_matrix * binormal_vectors[1],
fig->re_model_matrix * binormal_vectors[2] };

	transformed_tangential_vectors = { fig->re_model_matrix * tangential_vectors[0],
 fig->re_model_matrix * tangential_vectors[1],
fig->re_model_matrix * tangential_vectors[2] };

	after_transformations[0] /= after_transformations[0].w;
	after_transformations[1] /= after_transformations[1].w;
	after_transformations[2] /= after_transformations[2].w;
}