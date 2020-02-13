#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
	Camera(glm::vec3 pos, glm::vec3 forward, glm::vec3 up);
	~Camera() = default;

	glm::mat4 GetWorldMatrix() const { return m_world_matrix; }
	glm::mat4 GetProjectionMatrix() const { return m_projection_matrix; };
	glm::mat4 GetViewPortMatrix() const { return m_viewport_matrix; };

	void SetOrthographic(float left, float right, float top, float bottom, float near, float far);
	void SetFrustum(float left, float right, float top, float bottom, float near, float far);
	void SetPerspective(float fov, float aspect, float near, float far);
	void SetViewport(float offset_x, float offset_y, float screen_width, float screen_height);

	void LookAt(glm::vec3 pos, glm::vec3 front, glm::vec3 up);
	void SetPosFrontUp(glm::vec3& pos, glm::vec3& front, glm::vec3& up);
	void SetAngles(float& x, float& y);
	glm::vec3 pos;
	float x = -1.56f, y = 0.0f;

private:
	glm::vec3 m_position;
	glm::vec3 m_up_vector;
	glm::vec3 m_forward_vector;

	glm::vec3 front;
	glm::vec3 up;

	glm::mat4 m_world_matrix;
	glm::mat4 m_projection_matrix;
	glm::mat4 m_viewport_matrix;
};

