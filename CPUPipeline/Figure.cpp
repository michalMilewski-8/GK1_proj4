#include "Figure.h"

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
	case 1: return v.w - v.x;
		break;
	case 2: return v.w + v.x;
		break;
	case 3: return v.w - v.y;
		break;
	case 4: return v.w + v.y;
		break;
	case 5: return v.w - v.z;
		break;
	case 6: return v.w + v.z;
		break;
	default:
		return -1;
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
		bool all = true;
		for (int i = 0; i < 3; i++) {
			glm::vec4 A = after_transformations[i % 3];
			glm::vec4 B = after_transformations[(i + 1) % 3];
			add = true;
			for (int j = 1; j <= 6; j++) {
				float da = CalculateDist(A, j);
				float db = CalculateDist(B, j);

				if (da < 0 && db < 0) {
					add = false;
					all = false;
					break;
				}
				float dc = da / (da - db);
				if (da < 0) {
					all = false;
					A = A * (1 - dc) + B * dc;
				}
				if (db < 0) {
					all = false;
					B = A * (1 - dc) + B * dc;
				}
				
			}
			if (add) {
				points.push_back(A);
				points.push_back(B);
			}
		}

		points.erase(std::unique(points.begin(), points.end()), points.end());
	
		for (int k = 1; k < ((int)points.size() - 1); k++) 
		{
			auto tmp_ = points[0] / points[0].w;
			auto tmp2_ = points[k] / points[k].w;
			auto tmp3_ = points[k + 1] / points[k + 1].w;

	/*		auto tmp_ = after_transformations[0] / after_transformations[0].w;
			auto tmp2_ = after_transformations[1] / after_transformations[1].w;
			auto tmp3_ = after_transformations[1 + 1] / after_transformations[1 + 1].w;*/

			glm::vec3 tmp = { (tmp_.x+1)*1280/2,(tmp_.y + 1) * 720 / 2,(tmp_.z+1)/2 };
			glm::vec3 tmp2 = { (tmp2_.x + 1) * 1280 / 2,(tmp2_.y + 1) * 720 / 2,(tmp2_.z + 1) / 2 };
			glm::vec3 tmp3 = { (tmp3_.x + 1) * 1280 / 2,(tmp3_.y + 1) * 720 / 2,(tmp3_.z + 1) / 2 };
			//auto tmp = points[0] / points[0].w;
			//auto tmp2 = points[k] / points[k].w;
			//auto tmp3 = points[k + 1] / points[k + 1].w;
			

			if (paint_triangles)
			{
				//z_bufferng = false;
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

	//after_transformations[0] /= after_transformations[0].w;
	//after_transformations[1] /= after_transformations[1].w;
	//after_transformations[2] /= after_transformations[2].w;
}

void Figure::Transform(glm::mat4 proj, glm::mat4 view) {
	transformation = proj * view * scale * translate * rotate * center;
	/*modelT_1 = scale * translate * rotate * center;
	modelT_1 = glm::transpose(modelT_1);
	modelT_1 = glm::inverse(modelT_1);*/
}

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

void Cube::CalculateNormalVectors(unsigned int first, unsigned int second, unsigned int third, Triangle* tri)
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

Sphere::Sphere()
{
	type = Type::Sphere_;
	lattitude = 10;
	longitude = 10;
	r = 1.0f;
	CalculateVerticesAndTriangles();
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

	tri->normal_vectors.push_back({ glm::cross(glm::vec3(tri->tangential_vectors[0]),glm::vec3(tri->binormal_vectors[0])),1 });
	tri->normal_vectors.push_back({ glm::cross(glm::vec3(tri->tangential_vectors[1]),glm::vec3(tri->binormal_vectors[1])),1 });
	tri->normal_vectors.push_back({ glm::cross(glm::vec3(tri->tangential_vectors[2]),glm::vec3(tri->binormal_vectors[2])),1 });
}

Cylinder::Cylinder()
{
	type = Type::Cylinder_;
	longitude = 10;
	r = 1.0f;
	height = 2.0f;
	CalculateVerticesAndTriangles();
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

Cone::Cone()
{
	type = Type::Cone_;
	height = 1.0f;
	longitude = 10;
	r = 1.0f;
	CalculateVerticesAndTriangles();
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
