#include "../Light.h"

float Light::CalculateAttenuatiom(glm::vec3 to)
{
	float dist = glm::length(position - to);
	return 1/(Ac+Al*dist+Aq*dist*dist);
}

glm::vec3 Light::CalculateValueLightVal(glm::vec3 to, glm::vec3 N, glm::vec3 kd, float ks, int n, glm::vec3 cam_pos)
{
	auto L = position - to;
	L = glm::normalize(L);
	N = glm::normalize(N);
	auto tt = glm::dot(L, N);
	glm::vec3 result =  (tt * Id) * kd;
	auto R =  (glm::dot(L, N) * N * 2.0f - L);
	R = glm::normalize(R);
	auto V = cam_pos - to;
	V = glm::normalize(V);
	auto tmp2 = glm::dot(R, V);
	float tmp = std::pow(tmp2,n);

	if (tmp2 < 0 && tmp>0)
		tmp *= -1;

	result += (tmp * Is) * ks;
	result *= CalculateAttenuatiom(to);
	auto C = result;
	C.x = C.x > 1 ? 1 : C.x;
	C.y = C.y > 1 ? 1 : C.y;
	C.z = C.z > 1 ? 1 : C.z;
	C.x = C.x < 0 ? 0 : C.x;
	C.y = C.y < 0 ? 0 : C.y;
	C.z = C.z < 0 ? 0 : C.z;
	return C;
}
