
#include "window.hpp"


Window::Window(const glm::ivec2 dimensions, const char * const title) :
	w(glfwCreateWindow(dimensions.x, dimensions.y, title, nullptr, nullptr)) {}

Window::Window(const glm::ivec2 dimensions, const char * const title, const Window &share) :
	w(glfwCreateWindow(dimensions.x, dimensions.y, title, nullptr, share.w)) {}

void Window::bind() const {
	glfwMakeContextCurrent(this->w);
}

bool Window::should_close() const {
	return glfwWindowShouldClose(this->w);
}

void Window::swap_buffers() const {
	glfwSwapBuffers(this->w);
}

void Window::mark_to_close() const {
	glfwSetWindowShouldClose(this->w, true);
}

glm::ivec2 Window::get_dimensions() const {
	glm::ivec2 dimensions;
	glfwGetWindowSize(this->w, &dimensions.x, &dimensions.y);
	return dimensions;
}

glm::dvec2 Window::get_mouse_pos() const {
	glm::dvec2 position;
	glfwGetCursorPos(this->w, &position.x, &position.y);
	return position;
}

gl::KeyStatus Window::get_key_status(const gl::Key key) const {
	return (gl::KeyStatus) glfwGetKey(this->w, (int) key);
}

void Window::set_cursor_mode(const gl::CursorMode mode) const {
	glfwSetInputMode(this->w, GLFW_CURSOR, (int) mode);
}
