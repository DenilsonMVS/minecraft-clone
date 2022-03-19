
#include "player.hpp"

#include "chunk_definitions.hpp"


Player::Player() :
	position(0, 0, 0) {}

void Player::update(const float d_t, const Window &window) {

	this->camera.update(window.get_mouse_pos(), d_t);


	const auto facing_zero_y_normalized = glm::normalize(glm::vec3(this->camera.front.x, 0.0f, this->camera.front.z));
	
	if(window.get_key_status(gl::Key::W) == gl::KeyStatus::PRESS)
		this->position += this->speed * facing_zero_y_normalized * d_t;
	
	if(window.get_key_status(gl::Key::S) == gl::KeyStatus::PRESS)
		this->position -= this->speed * facing_zero_y_normalized * d_t;
	
	if(window.get_key_status(gl::Key::A) == gl::KeyStatus::PRESS)
		this->position -= this->speed * glm::normalize(glm::cross(facing_zero_y_normalized, glm::vec3(0.0f, 1.0f, 0.0f))) * d_t;

	if(window.get_key_status(gl::Key::D) == gl::KeyStatus::PRESS)
		this->position += this->speed * glm::normalize(glm::cross(facing_zero_y_normalized, glm::vec3(0.0f, 1.0f, 0.0f))) * d_t;

	if(window.get_key_status(gl::Key::SPACE) == gl::KeyStatus::PRESS)
		this->position.y += this->speed * d_t;

	if(window.get_key_status(gl::Key::LEFT_SHIFT) == gl::KeyStatus::PRESS)
		this->position.y -= this->speed * d_t;

	const glm::ivec3 chunk_pos = get_chunk_pos_based_on_block_inside(det::to_int(this->position));
	this->camera.position = this->position - glm::dvec3(chunk_pos * chunk_size);
}

const float Player::range = 3;
