
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "window.hpp"
#include "camera.hpp"


struct Player {
	Player();

	void update(const float d_t, const Window &window);

	Camera camera;
	glm::vec3 position;
	float speed;

	static const float range;
};

#endif
