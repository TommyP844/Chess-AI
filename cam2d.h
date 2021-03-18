#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Camera
{
public:
	Camera();
	~Camera();

	void update_camera_position(int dx, int dy, int dz);
	void update_window_dimensions(int sw, int sh);

	inline mat4 get_ortho_matrix() const { return m_ortho_matrix; }
	inline mat4 get_view_matrix() const { return m_view_matrix; }
	inline mat4 get_view_ortho_matrix() const { return m_view_proj_matrix; }

private:
	mat4 m_ortho_matrix;
	mat4 m_view_matrix;
	mat4 m_view_proj_matrix;

	vec3 m_position;

};
