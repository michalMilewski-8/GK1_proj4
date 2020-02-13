#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(glm::vec3 pos, glm::vec3 forward, glm::vec3 up)
{
	LookAt(pos, forward, up);
}

void Camera::SetOrthographic(float left, float right, float top, float bottom, float near, float far)
{

}

void Camera::SetFrustum(float left, float right, float top, float bottom, float near, float far)
{

}

void Camera::SetPerspective(float fov_y, float aspect, float near, float far)
{
	glm::mat4 persp = glm::mat4(1.0f);
	float e = 1 / tan(fov_y / 2);
	persp[0][0] = e;
	persp[1][1] = e / aspect;
	persp[2][2] = -((far + near) / (far - near));
	persp[3][2] = -((far * near * 2) / (far - near));
	persp[3][3] = 0;
	persp[2][3] = -1;
	m_projection_matrix = persp;

}

void Camera::SetViewport(float offset_x, float offset_y, float screen_width, float screen_height)
{
	glm::mat4 trans = glm::mat4(1.0f);
	trans[3][0] = offset_x + screen_width / 2;
	trans[3][1] = offset_y + screen_height / 2;

	glm::mat4 scale = glm::mat4(1.0f);
	scale[0][0] = screen_width / 2;
	scale[1][1] = screen_height / 2;
	scale[2][2] = 1.0f / 2;

	m_viewport_matrix = trans * scale;
}

void Camera::LookAt(glm::vec3 pos, glm::vec3 front, glm::vec3 up)
{
	this->pos = pos;
	this->front = front;
	this->up = up;

	glm::vec3 const f(glm::normalize(front));
	glm::vec3 const s(glm::normalize(cross(f, up)));
	glm::vec3 const u(glm::cross(s, f));

	glm::mat4 Result(1.0f);
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	Result[0][2] = -f.x;
	Result[1][2] = -f.y;
	Result[2][2] = -f.z;
	Result[3][0] = -glm::dot(s, pos);
	Result[3][1] = -glm::dot(u, pos);
	Result[3][2] = glm::dot(f, pos);
	m_world_matrix = Result;
	//m_world_matrix = glm::lookAt(pos, pos + front, up);
}

void Camera::SetPosFrontUp(glm::vec3& pos, glm::vec3& front, glm::vec3& up)
{
	pos = this->pos;
	front = this->front;
	up = this->up;
}

void Camera::SetAngles(float& x, float& y)
{
	x = this->x;
	y = this->y;
}
