
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "window.hpp"
#include "camera.hpp"


struct Player {
	void update(const float d_t, const Window &window);

	Camera camera;
};

#endif
