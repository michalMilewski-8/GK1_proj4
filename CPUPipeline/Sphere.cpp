#include "../Sphere.h"
#include "imgui.h"
Sphere::Sphere()
{
	type = Type::Sphere_;
	lattitude = 10;
	longitude = 10;
	r = 1.0f;
	CalculateVerticesAndTriangles();
}

void Sphere::ModMenu(int counter, Figure*& to_delete)
{
	char buff[500];
	sprintf_s(buff, 500, "Figure #%d : Sphere", counter);
	if (ImGui::TreeNode(buff)) {
	Figure::ModMenu(counter, to_delete);
	auto sphere = this;
	int longitude, lattitude;
	longitude = sphere->longitude;
	lattitude = sphere->lattitude;

	ImGui::Text("Number of divistions");
	ImGui::SliderInt("Longitude", &longitude, 1, 100);
	ImGui::SliderInt("Lattitude", &lattitude, 1, 100);
	ImGui::Spacing();
	float R;
	R = sphere->r;
	ImGui::SliderFloat("R", &R, 0.01f, 10.0f);

	if (sphere->lattitude != lattitude || sphere->longitude != longitude || sphere->r != R) {
		sphere->lattitude = lattitude;
		sphere->longitude = longitude;
		sphere->r = R;
		sphere->CalculateVerticesAndTriangles();
	}
	if (ImGui::Button("Delete"))
		to_delete = this;
	ImGui::TreePop();
	ImGui::Separator();
	}
}

void Sphere::CalculateVerticesAndTriangles()
{
	vertices.clear();
	triangles.clear();
	float lattitude_step = M_PI / (((float)lattitude) - 1);
	float longitude_step = (2 * M_PI) / (float)(longitude);
	float lon_val = 0;
	float latt_val = 0;
	for (int i = 0; i < longitude; i++) {
		latt_val = 0;
		for (int j = 0; j < lattitude; j++) {

			glm::vec4 point = glm::vec4(r * sin(latt_val) * cos(lon_val), r * cos(latt_val), r * sin(latt_val) * sin(lon_val), 1);

			vertices.push_back(point);

			latt_val += lattitude_step;
		}
		lon_val += longitude_step;
	}
	int val = 0;
	for (int i = 0; i < longitude - 1; i++) {
		for (int j = 0; j < lattitude - 1; j++) {
			triangles.push_back(Triangle(i * lattitude + j, (i + 1) * lattitude + j, i * lattitude + j + 1, this));
			triangles.push_back(Triangle(i * lattitude + j + 1, (i + 1) * lattitude + j, (i + 1) * lattitude + j + 1, this));
		}
	}
	for (int j = 0; j < lattitude - 1; j++) {
		triangles.push_back(Triangle((longitude - 1) * lattitude + j, j, (longitude - 1) * lattitude + j + 1, this));
		triangles.push_back(Triangle((longitude - 1) * lattitude + j + 1, j, j + 1, this));
	}
}

void Sphere::CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri)
{
	tri->binormal_vectors.clear();
	tri->normal_vectors.clear();
	tri->tangential_vectors.clear();
	int i, j;
	float lon_val = 0;
	float latt_val = 0;
	float lattitude_step = M_PI / (float)lattitude;
	float longitude_step = 2 * M_PI / (float)longitude;

	i = first / lattitude;
	j = first % lattitude;

	lon_val = i * longitude_step;
	latt_val = j * lattitude_step;

	tri->tangential_vectors.push_back({ r * cos(latt_val) * cos(lon_val),r * -sin(latt_val),r * cos(latt_val) * sin(lon_val), 1 });
	tri->binormal_vectors.push_back({ r * sin(latt_val) * -sin(lon_val),r * cos(latt_val),r * sin(latt_val) * cos(lon_val), 1 });

	i = second / lattitude;
	j = second % lattitude;

	lon_val = i * longitude_step;
	latt_val = j * lattitude_step;

	tri->tangential_vectors.push_back({ r * cos(latt_val) * cos(lon_val),r * -sin(latt_val),r * cos(latt_val) * sin(lon_val), 1 });
	tri->binormal_vectors.push_back({ r * sin(latt_val) * -sin(lon_val),r * cos(latt_val),r * sin(latt_val) * cos(lon_val), 1 });

	i = third / lattitude;
	j = third % lattitude;

	lon_val = i * longitude_step;
	latt_val = j * lattitude_step;

	tri->tangential_vectors.push_back({ r * cos(latt_val) * cos(lon_val),r * -sin(latt_val),r * cos(latt_val) * sin(lon_val), 1 });
	tri->binormal_vectors.push_back({ r * sin(latt_val) * -sin(lon_val),r * cos(latt_val),r * sin(latt_val) * cos(lon_val), 1 });

	/*tri->normal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->binormal_vectors[0]),glm::vec3(tri->tangential_vectors[0]))),1 });
	tri->normal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->binormal_vectors[1]),glm::vec3(tri->tangential_vectors[1]))),1 });
	tri->normal_vectors.push_back({ glm::normalize(glm::cross(glm::vec3(tri->binormal_vectors[2]),glm::vec3(tri->tangential_vectors[2]))),1 });*/
	tri->normal_vectors.push_back(glm::normalize(vertices[tri->vertices_ind[0]]));
	tri->normal_vectors.push_back(glm::normalize(vertices[tri->vertices_ind[1]]));
	tri->normal_vectors.push_back(glm::normalize(vertices[tri->vertices_ind[2]]));

}