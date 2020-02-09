#include "../Cylinder.h"
#include "imgui.h"

static const glm::vec3 up(0, -1, 0);
static const glm::vec3 up2(0, -1, 0.001);

Cylinder::Cylinder()
{
	type = Type::Cylinder_;
	longitude = 10;
	r = 1.0f;
	height = 2.0f;
	CalculateVerticesAndTriangles();
}

void Cylinder::ModMenu(int counter, Figure*& to_delete)
{
	char buff[500];
	sprintf_s(buff, 500, "Figure #%d : Cylinder", counter);
	if (ImGui::TreeNode(buff)) {
	Figure::ModMenu(counter,to_delete);
	auto cone = this;
	int longitude;
	longitude = cone->longitude;

	ImGui::Text("Number of divistions");
	ImGui::SliderInt("Longitude", &longitude, 1, 100);
	ImGui::Spacing();
	float R, H;
	R = cone->r;
	H = cone->height;
	ImGui::SliderFloat("Radius", &R, 0.01f, 10.0f);
	ImGui::SliderFloat("Height", &H, 0.01f, 10.0f);

	if (cone->height != H || cone->longitude != longitude || cone->r != R) {
		cone->height = H;
		cone->longitude = longitude;
		cone->r = R;
		cone->CalculateVerticesAndTriangles();
	}
	if (ImGui::Button("Delete"))
		to_delete = this;
	ImGui::TreePop();
	ImGui::Separator();
	}
}

void Cylinder::CalculateVerticesAndTriangles()
{
	vertices.clear();
	triangles.clear();
	float longitude_step = 2 * M_PI / ((float)longitude);
	float lon_val = 0;
	glm::vec4 point;

	for (int i = 0; i < longitude; i++) {
		point = glm::vec4(r * cos(lon_val), -height / 2, r * -sin(lon_val), 1);
		vertices.push_back(point);

		point = glm::vec4(r * cos(lon_val), height / 2, r * -sin(lon_val), 1);
		vertices.push_back(point);
		lon_val += longitude_step;
	}
	vertices.push_back(glm::vec4(0.0f, -height / 2, 0.0f, 1));
	vertices.push_back(glm::vec4(0.0f, height / 2, 0.0f, 1));
	for (int i = 0; i < 2 * (longitude - 1); i += 2) {
		triangles.push_back(Triangle(i, 2 * longitude, (i + 2), this));
		triangles.push_back(Triangle(i + 1, (i + 3), 2 * longitude + 1, this));
		triangles.push_back(Triangle(i, i + 2, (i + 1), this));
		triangles.push_back(Triangle(i + 1, i + 2, (i + 3), this));
	}
	triangles.push_back(Triangle(2 * longitude - 2, 2 * longitude, 0, this));
	triangles.push_back(Triangle(2 * longitude - 1, 1, 2 * longitude + 1, this));
	triangles.push_back(Triangle(2 * longitude - 2, 0, 2 * longitude - 1, this));
	triangles.push_back(Triangle(2 * longitude - 1, 0, 1, this));
}

void Cylinder::CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri)
{
	tri->binormal_vectors.clear();
	tri->normal_vectors.clear();
	tri->tangential_vectors.clear();
	auto one = vertices[first];
	auto two = vertices[second];
	auto three = vertices[third];

	if (one.y == three.y && one.y == two.y) {

		glm::vec4 center = { 0,one.y,0,1 };

		glm::vec3 one_ = (two - one);
		glm::vec3 two_ = (three - one);
		auto tmp = glm::cross(one_, two_);
		tri->normal_vectors.push_back(glm::vec4(tmp, 0));
		glm::vec3 binormal = glm::normalize(one - center);
		tri->binormal_vectors.push_back({ binormal ,0 });
		tri->tangential_vectors.push_back({glm::normalize(glm::cross(tmp,binormal)),0});
		one_ = (three - two);
		two_ = (one - two);
		tmp = glm::cross(one_, two_);
		tri->normal_vectors.push_back(glm::vec4(tmp.x, tmp.y, tmp.z, 0));
		binormal = glm::normalize(two - center);
		tri->binormal_vectors.push_back({ binormal ,0 });
		tri->tangential_vectors.push_back({ glm::normalize(glm::cross(tmp,binormal)),0 });

		one_ = (one - three);
		two_ = (two - three);
		tmp = glm::cross(one_, two_);
		tri->normal_vectors.push_back(glm::vec4(tmp.x, tmp.y, tmp.z, 0));
		binormal = glm::normalize(three - center);
		tri->binormal_vectors.push_back({ binormal ,0 });
		tri->tangential_vectors.push_back({ glm::normalize(glm::cross(tmp,binormal)),0 });
	}if (one.y == two.y) {
		glm::vec4 center = { 0,one.y,0,1 };
		glm::vec4 center2 = { 0,three.y,0,1 };
		tri->normal_vectors.push_back(one-center);
		tri->normal_vectors.push_back(two-center);
		std::swap(center , center2);
		tri->normal_vectors.push_back(three-center);

		glm::vec3 one_t = center2 - center;
		glm::vec3 three_t = center-center2;

		tri->tangential_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[0]),one_t)),0 });
		tri->tangential_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[1]),one_t)),0 });
		tri->tangential_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[2]),three_t)),0 });

		tri->binormal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[0]),glm::vec3(tri->tangential_vectors[0]))),0 });
		tri->binormal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[1]),glm::vec3(tri->tangential_vectors[1]))),0 });
		tri->binormal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[2]),glm::vec3(tri->tangential_vectors[2]))),0 });


	}
	else if (two.y == three.y) {
		glm::vec4 center = { 0,one.y,0,1 };
		glm::vec4 center2 = { 0,three.y,0,1 };
		tri->normal_vectors.push_back(one - center);
		std::swap(center, center2);
		tri->normal_vectors.push_back(two - center);
		tri->normal_vectors.push_back(three - center);

		glm::vec3 one_t = center2 - center;
		glm::vec3 three_t = center - center2;

		tri->tangential_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[0]),one_t)),0 });
		tri->tangential_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[1]),three_t)),0 });
		tri->tangential_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[2]),three_t)),0 });

		tri->binormal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[0]),glm::vec3(tri->tangential_vectors[0]))),0 });
		tri->binormal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[1]),glm::vec3(tri->tangential_vectors[1]))),0 });
		tri->binormal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[2]),glm::vec3(tri->tangential_vectors[2]))),0 });
	}
	else {
		glm::vec4 center = { 0,one.y,0,1 };
		glm::vec4 center2 = { 0,two.y,0,1 };
		tri->normal_vectors.push_back(one - center);
		std::swap(center, center2);
		tri->normal_vectors.push_back(two - center);
		std::swap(center, center2);
		tri->normal_vectors.push_back(three - center);

		glm::vec3 one_t = center2 - center;
		glm::vec3 three_t = center - center2;

		tri->tangential_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[0]),three_t)),0 });
		tri->tangential_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[1]),one_t)),0 });
		tri->tangential_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[2]),three_t)),0 });

		tri->binormal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[0]),glm::vec3(tri->tangential_vectors[0]))),0 });
		tri->binormal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[1]),glm::vec3(tri->tangential_vectors[1]))),0 });
		tri->binormal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[2]),glm::vec3(tri->tangential_vectors[2]))),0 });
	}

	tri->tangential_vectors.clear();
	tri->binormal_vectors.clear();

	glm::vec3 tmp;
	for (int i = 0; i < 3; i++) {
		if (tri->normal_vectors[i].x == 0 && tri->normal_vectors[i].z == 0) {

			tmp = glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[i]), up2));
			tri->tangential_vectors.push_back(glm::vec4(tmp.x, tmp.y, tmp.z, 0));
		}
		else {
			tmp = glm::normalize(glm::cross(glm::vec3(tri->normal_vectors[i]), up));
			tri->tangential_vectors.push_back(glm::vec4(tmp.x, tmp.y, tmp.z, 0));
		}
		tmp = glm::normalize(glm::cross(glm::vec3(tri->tangential_vectors[i]), glm::vec3(tri->normal_vectors[i])));
		tri->binormal_vectors.push_back(glm::vec4(tmp.x, tmp.y, tmp.z, 0));
	}

}
