
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


class Camera {
public:
	Camera(const float sensitivity = 1.0f, const float speed = 3.0f);

	void update(const glm::vec2 mouse_pos, const float d_time);
	glm::mat4 get_view_projection(
		const glm::ivec2 window_dimensions, 
		const float fov, 
		const float near_distance = 0.01f, 
		const float far_distance = 1000.f) const;

	glm::vec3 position;
	glm::vec3 front;
	float sensitivity, speed;

private:
	glm::vec2 last_mouse_pos;
	float pitch, yaw;
};

#endif
