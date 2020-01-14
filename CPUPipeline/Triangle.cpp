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

void Triangle::DrawTriangle(bool backface_culling, bool paint_triangles, bool z_bufferng, bool perspective_correction, FrameBuffer& fb, int color, Camera* cam)
{
	float val = 1.0f;
	if (backface_culling)
		val = CalculateSideValue();
	if (val >= 0) {
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

				points.push_back(A);
				if (i != 2)
					points.push_back(B);
			}
		}

		points.erase(std::unique(points.begin(), points.end()), points.end());

		for (int k = 1; k < ((int)points.size() - 1); k++) {
			auto tmp_ = points[0] / points[0].w;
			auto tmp2_ = points[k] / points[k].w;
			auto tmp3_ = points[k + 1] / points[k + 1].w;

			//auto tmp_ = points[0] ;
			//auto tmp2_ = points[k];
			//auto tmp3_ = points[k + 1] ;

			auto tmp = cam->GetViewPortMatrix() * tmp_;
			auto tmp2 = cam->GetViewPortMatrix() * tmp2_;
			auto tmp3 = cam->GetViewPortMatrix() * tmp3_;

			//tmp /= tmp.w;
			//tmp2 /= tmp2.w;
			//tmp3 /= tmp3.w;
			//auto tmp = points[0] / points[0].w;
			//auto tmp2 = points[k] / points[k].w;
			//auto tmp3 = points[k + 1] / points[k + 1].w;


			if (paint_triangles)
			{
				if (z_bufferng)
					fb.FillTriangle(tmp.x, tmp.y, tmp.z, tmp2.x, tmp2.y, tmp2.z, tmp3.x, tmp3.y, tmp3.z, color);
				else
					fb.FillTriangle(tmp.x, tmp.y, tmp2.x, tmp2.y, tmp3.x, tmp3.y, color);

			}
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

	//	transformed_normal_vectors = { fig->transformation * normal_vectors[0],
	// fig->transformation * normal_vectors[1],
	//fig->transformation * normal_vectors[2] };
	//
	//	transformed_binormal_vectors = { fig->transformation * binormal_vectors[0],
	// fig->transformation * binormal_vectors[1],
	//fig->transformation * binormal_vectors[2] };
	//
	//	transformed_tangential_vectors = { fig->transformation * tangential_vectors[0],
	// fig->transformation * tangential_vectors[1],
	//fig->transformation * tangential_vectors[2] };

		//transformed_normal_vectors[0] /= transformed_normal_vectors[0].w;
		//transformed_normal_vectors[1] /= transformed_normal_vectors[1].w;
		//transformed_normal_vectors[2] /= transformed_normal_vectors[2].w;

		//transformed_binormal_vectors[0] /= transformed_binormal_vectors[0].w;
		//transformed_binormal_vectors[1] /= transformed_binormal_vectors[1].w;
		//transformed_binormal_vectors[2] /= transformed_binormal_vectors[2].w;

		//transformed_tangential_vectors[0] /= transformed_tangential_vectors[0].w;
		//transformed_tangential_vectors[1] /= transformed_tangential_vectors[1].w;
		//transformed_tangential_vectors[2] /= transformed_tangential_vectors[2].w;

	after_transformations[0] /= after_transformations[0].w;
	after_transformations[1] /= after_transformations[1].w;
	after_transformations[2] /= after_transformations[2].w;
}