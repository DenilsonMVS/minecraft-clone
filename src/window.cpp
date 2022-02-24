
#include "window.hpp"


Window::Window(const int width, const int height, const char * const title) :
	w(glfwCreateWindow(width, height, title, nullptr, nullptr)) {}

Window::Window(const int width, const int height, const char * const title, const Window &share) :
	w(glfwCreateWindow(width, height, title, nullptr, share.w)) {}

void Window::bind() const {
	glfwMakeContextCurrent(this->w);
}

bool Window::should_close() const {
	return glfwWindowShouldClose(this->w);
}

void Window::swap_buffers() const {
	glfwSwapBuffers(this->w);
}

glm::ivec2 Window::get_dimensions() const {
	glm::ivec2 dimensions;
	glfwGetWindowSize(this->w, &dimensions.x, &dimensions.y);
	return dimensions;
}
