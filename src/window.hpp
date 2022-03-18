
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "utils.hpp"
#include "gl.hpp"


class Window {
public:
	friend class Renderer;

	void bind() const;
	bool should_close() const;
	void swap_buffers() const;
	void mark_to_close() const;

	glm::ivec2 get_dimensions() const;
	glm::dvec2 get_mouse_pos() const;
	gl::KeyStatus get_key_status(const gl::Key key) const;
	gl::KeyStatus get_mouse_button_status(const gl::MouseButton button) const;

	void set_cursor_mode(const gl::CursorMode mode) const;
	
private:
	Window(const glm::ivec2 &dimensions, const char * const title);
	Window(const glm::ivec2 &dimensions, const char * const title, const Window &share);

	GLFWwindow *w;
};


#endif
