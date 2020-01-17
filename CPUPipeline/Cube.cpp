#include "../Cube.h"
#include "imgui.h"

Cube::Cube(std::vector<glm::vec4> vert)
{
	type = Type::Cube_;
	vertices = vert;
	triangles = { Triangle(0,1,2,this),Triangle(2,3,0,this),Triangle(3,2,6,this)
	,Triangle(6,7,3,this) ,Triangle(6,5,4,this) ,Triangle(6,4,7,this)
	,Triangle(5,1,0,this) ,Triangle(5,0,4,this) ,Triangle(1,5,2,this)
	,Triangle(5,6,2,this) ,Triangle(0,3,4,this) ,Triangle(4,3,7,this) };
	CalculateEdgesLengths();

}

Cube::Cube()
{
	type = Type::Cube_;
	vertices = std::vector<glm::vec4>{
		{0.0f, 0.0f, 0.0f, 1.0f}, { 0.0f,1.0f,0.0f,1.0f }, { 1.0f,1.0f,0.0f,1.0f }, { 1.0f,0.0f,0.0f,1.0f },
		{ 0.0f,0.0f,1.0f,1.0f }, { 0.0f,1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 1.0f,0.0f,1.0f,1.0f }
	};

	triangles = { Triangle(0,1,2,this),Triangle(2,3,0,this),Triangle(3,2,6,this)
	,Triangle(6,7,3,this) ,Triangle(6,5,4,this) ,Triangle(6,4,7,this)
	,Triangle(5,1,0,this) ,Triangle(5,0,4,this) ,Triangle(1,5,2,this)
	,Triangle(5,6,2,this) ,Triangle(0,3,4,this) ,Triangle(4,3,7,this) };
	CalculateEdgesLengths();
}

void Cube::ModMenu(int counter, Figure*& to_delete)
{
	char buff[500];
	sprintf_s(buff, 500, "Figure #%d : Cube", counter);
	if (ImGui::TreeNode(buff)) {
	Figure::ModMenu(counter, to_delete);
	float lengths[12];
	for (int i = 0; i < 12; i++) {
		lengths[i] = this->edges_lengts[i];
		sprintf_s(buff, 100, "Length %d", i);
		ImGui::SliderFloat(buff, &lengths[i], 0.0f, 5.0f);
	}
	bool difference = false;
	int diff_index = 0;
	for (int i = 0; i < 12; i++) {
		if (this->edges_lengts[i] != lengths[i]) {
			difference = true;
			diff_index = i;
		}
	}
	if (difference) {
		this->ChangeEdgeLength(diff_index, lengths[diff_index]);
	}
	if (ImGui::Button("Delete"))
		to_delete = this;
	ImGui::TreePop();
	ImGui::Separator();
	}
}

void Cube::CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri)
{
	tri->binormal_vectors.clear();
	tri->normal_vectors.clear();
	tri->tangential_vectors.clear();
	auto one = vertices[first];
	auto two = vertices[second];
	auto three = vertices[third];
	glm::vec3 tangenial;
	glm::vec3 binormal;

	int which = -1;

	glm::vec3 one_ = (two - one);
	glm::vec3 two_ = (three - one);
	auto tmp = glm::cross(one_, two_);
	if (glm::dot(one_, two_) == 0)
	{
		which = 1;
		tangenial = -one_;
		binormal = -two_;
		if (glm::cross(tangenial, binormal) != tmp) {
			std::swap(tangenial, binormal);
		}
	}
	tri->normal_vectors.push_back(glm::vec4(tmp.x, tmp.y, tmp.z, 0));

	one_ = (three - two);
	two_ = (one - two);
	tmp = glm::cross(one_, two_);
	if (glm::dot(one_, two_) == 0)
	{
		which = 2;
		tangenial = -one_;
		binormal = -two_;
		if (glm::cross(tangenial, binormal) != tmp) {
			std::swap(tangenial, binormal);
		}
	}
	tri->normal_vectors.push_back(glm::vec4(tmp.x, tmp.y, tmp.z, 0));

	one_ = (one - three);
	two_ = (two - three);
	tmp = glm::cross(one_, two_);
	if (glm::dot(one_, two_) == 0)
	{
		which = 3;
		tangenial = -one_;
		binormal = -two_;
		if (glm::cross(tangenial, binormal) != tmp) {
			std::swap(tangenial, binormal);
		}
	}
	tri->normal_vectors.push_back(glm::vec4(tmp.x, tmp.y, tmp.z, 0));

	if (which == 1) {
		tri->tangential_vectors.push_back({ tangenial ,0 });
		tri->tangential_vectors.push_back({ -binormal ,0 });
		tri->tangential_vectors.push_back({ binormal ,0 });
	}
	else if (which == 2) {
		tri->tangential_vectors.push_back({ binormal ,0 });
		tri->tangential_vectors.push_back({ tangenial ,0 });
		tri->tangential_vectors.push_back({ -binormal ,0 });
	}
	else if (which == 3) {
		tri->tangential_vectors.push_back({ -binormal ,0 });
		tri->tangential_vectors.push_back({ binormal ,0 });
		tri->tangential_vectors.push_back({ tangenial ,0 });
	}

	tri->binormal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->tangential_vectors[0]), glm::vec3(tri->normal_vectors[0]))),0 });
	tri->binormal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->tangential_vectors[1]), glm::vec3(tri->normal_vectors[1]))),0 });
	tri->binormal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->tangential_vectors[2]), glm::vec3(tri->normal_vectors[2]))),0 });

}

void Cube::CalculateEdgesLengths()
{
	auto tmp = vertices[1] - vertices[0];
	edges_lengts[0] = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	tmp = vertices[3] - vertices[0];
	edges_lengts[1] = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	tmp = vertices[4] - vertices[0];
	edges_lengts[2] = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	tmp = vertices[1] - vertices[2];
	edges_lengts[3] = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	tmp = vertices[3] - vertices[2];
	edges_lengts[4] = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	tmp = vertices[6] - vertices[2];
	edges_lengts[5] = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	tmp = vertices[1] - vertices[5];
	edges_lengts[6] = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	tmp = vertices[4] - vertices[5];
	edges_lengts[7] = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	tmp = vertices[6] - vertices[5];
	edges_lengts[8] = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	tmp = vertices[3] - vertices[7];
	edges_lengts[9] = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	tmp = vertices[4] - vertices[7];
	edges_lengts[10] = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	tmp = vertices[6] - vertices[7];
	edges_lengts[11] = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
}

void Cube::ChangeEdgeLength(int edge, float new_length)
{
	glm::vec4 tmp;
	glm::vec4* first;
	glm::vec4* second;
	switch (edge)
	{
	case 0:
		/*tmp = vertices[1] - vertices[0];
		float len = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
		tmp /= len / new_length;
		vertices[1] = vertices[0] + tmp;*/
		first = &vertices[1];
		second = &vertices[0];
		break;
	case 1:
		first = &vertices[3];
		second = &vertices[0];
		break;
	case 2:
		first = &vertices[4];
		second = &vertices[0];
		break;
	case 3:
		first = &vertices[1];
		second = &vertices[2];
		break;
	case 4:
		first = &vertices[3];
		second = &vertices[2];
		break;
	case 5:
		first = &vertices[6];
		second = &vertices[2];
		break;
	case 6:
		first = &vertices[1];
		second = &vertices[5];
		break;
	case 7:
		first = &vertices[4];
		second = &vertices[5];
		break;
	case 8:
		first = &vertices[6];
		second = &vertices[5];
		break;
	case 9:
		first = &vertices[3];
		second = &vertices[7];
		break;
	case 10:
		first = &vertices[4];
		second = &vertices[7];
		break;
	case 11:
		first = &vertices[6];
		second = &vertices[7];
		break;
	default:
		first = &vertices[0];
		second = &vertices[0];
		break;
	}

	tmp = *first - *second;
	float len = std::sqrtf(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	tmp /= len / new_length;
	*first = *second + tmp;

	CalculateEdgesLengths();
}