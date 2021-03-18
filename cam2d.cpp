
#include "cam2d.h"
Camera::Camera()
	:
	m_ortho_matrix(mat4()),
	m_position(vec3()),
	m_view_matrix(mat4()),
	m_view_proj_matrix(mat4())
{
}

Camera::~Camera()
{
}

void Camera::update_camera_position(int dx, int dy, int dz)
{
	m_position.x = (float)dx;
	m_position.y = (float)dy;
	m_position.z = (float)dz;

	m_view_matrix = glm::lookAt(m_position, { 0.f, 0.f, -1.f }, { 0.f, 1.f, 0.f });
	m_view_proj_matrix = m_ortho_matrix * m_view_matrix;

}

void Camera::update_window_dimensions(int sw, int sh)
{
	m_ortho_matrix = glm::ortho(0.f, (float)sw, (float)sh, 0.f, 1.f, 5.0f);
}

