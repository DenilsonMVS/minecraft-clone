
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "utils.hpp"

#include <glm/glm.hpp>


class Renderer;

class Window {
public:
	friend class Renderer;

	void bind() const;
	bool should_close() const;
	void swap_buffers() const;

	glm::ivec2 get_dimensions() const;

private:
	Window(const int width, const int height, const char * const title);
	Window(const int width, const int height, const char * const title, const Window &share);

	GLFWwindow *w;
};


#endif
