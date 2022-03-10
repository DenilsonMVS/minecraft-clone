
#include "camera.hpp"


Camera::Camera(const float sensitivity, const float speed) :
	position(0.0f, 0.0f, 0.0f),
	sensitivity(sensitivity), speed(speed),
	last_mouse_pos({0, 0}),
	pitch(0.0f), yaw(0.0f) {

	this->front = glm::normalize(glm::vec3(
		cosf(glm::radians(this->yaw)) * cosf(glm::radians(this->pitch)),
		sinf(glm::radians(this->pitch)),
		sinf(glm::radians(this->yaw)) * cosf(glm::radians(this->pitch))
	));
}

void Camera::update(const glm::vec2 mouse_pos, const float d_time) {
	if(mouse_pos == this->last_mouse_pos)
		return;

	const auto offset = mouse_pos - this->last_mouse_pos;
	this->yaw += offset.x * this->sensitivity * d_time;
	
	this->pitch -= offset.y * this->sensitivity * d_time;
	if(this->pitch > 89.9f)
		this->pitch = 89.9f;
	else if(this->pitch < -89.9f)
		this->pitch = -89.9f;

	this->front = glm::normalize(glm::vec3(
		cosf(glm::radians(this->yaw)) * cosf(glm::radians(this->pitch)),
		sinf(glm::radians(this->pitch)),
		sinf(glm::radians(this->yaw)) * cosf(glm::radians(this->pitch))
	));

	this->last_mouse_pos = mouse_pos;
}

glm::mat4 Camera::get_view_projection(const glm::ivec2 window_dimensions, const float fov, const float near_distance, const float far_distance) const {
	return glm::perspective(
		glm::radians(fov), 
		(float) window_dimensions.x / window_dimensions.y, 
		near_distance, far_distance)
	* glm::lookAt(
		this->position,
		this->position + this->front,
		glm::vec3(0.0f, 1.0f, 0.0f));
}
