#include "../Cone.h"
#include "imgui.h"

Cone::Cone()
{
	type = Type::Cone_;
	height = 1.0f;
	longitude = 10;
	r = 1.0f;
	CalculateVerticesAndTriangles();
}

void Cone::ModMenu(int counter, Figure*& to_delete)
{
	char buff[500];
	sprintf_s(buff, 500, "Figure #%d : Cone", counter);
	if (ImGui::TreeNode(buff)) {
	Figure::ModMenu(counter, to_delete);
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

void Cone::CalculateVerticesAndTriangles()
{
	vertices.clear();
	triangles.clear();
	float longitude_step = 2 * M_PI / ((float)longitude);
	float lon_val = 0;
	glm::vec4 point;

	for (int i = 0; i < longitude; i++) {
		point = glm::vec4(r * cos(lon_val), height / 2, r * -sin(lon_val), 1);
		vertices.push_back(point);

		lon_val += longitude_step;
	}
	vertices.push_back(glm::vec4(0.0f, height / 2, 0.0f, 1));
	vertices.push_back(glm::vec4(0.0f, -height / 2, 0.0f, 1));
	for (int i = 0; i < longitude - 1; i++) {
		triangles.push_back(Triangle(i, (i + 1), longitude, this));
		triangles.push_back(Triangle(i, longitude + 1, i + 1, this));
	}
	triangles.push_back(Triangle(longitude - 1, 0, longitude, this));
	triangles.push_back(Triangle(longitude - 1, longitude + 1, 0, this));

}

void Cone::CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri)
{
	tri->binormal_vectors.clear();
	tri->normal_vectors.clear();
	tri->tangential_vectors.clear();
	auto one = vertices[first];
	auto two = vertices[second];
	auto three = vertices[third];


	glm::vec3 one_ = (two - one);
	glm::vec3 two_ = (three - one);
	auto tmp = glm::cross(two_, one_);
	tri->normal_vectors.push_back(glm::vec4(tmp.x, tmp.y, tmp.z, 1));

	one_ = (three - two);
	two_ = (one - two);
	tmp = glm::cross(two_, one_);
	tri->normal_vectors.push_back(glm::vec4(tmp.x, tmp.y, tmp.z, 1));

	one_ = (one - three);
	two_ = (two - three);
	tmp = glm::cross(two_, one_);
	tri->normal_vectors.push_back(glm::vec4(tmp.x, tmp.y, tmp.z, 1));

	tri->tangential_vectors.push_back(tri->normal_vectors[1]);
	tri->tangential_vectors.push_back(tri->normal_vectors[2]);
	tri->tangential_vectors.push_back(tri->normal_vectors[0]);

	tri->binormal_vectors.push_back(tri->normal_vectors[2]);
	tri->binormal_vectors.push_back(tri->normal_vectors[0]);
	tri->binormal_vectors.push_back(tri->normal_vectors[1]);
}
