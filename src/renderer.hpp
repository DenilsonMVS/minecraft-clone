
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include "window.hpp"


class Renderer {
public:
	static Window create_window(const glm::ivec2 dimensions, const char * const title);
	static Window create_window(const glm::ivec2 dimensions, const char * const title, const Window &share);

	static void set_clear_color(const float r, const float g, const float b);
	static void set_clear_color(const glm::vec3 &color);
	static void poll_events();
	static void clear(const gl::BitField mask);

	static void use_wireframe();
	static void use_filling();

	static void enable(const gl::Capability cap);
	static void enable(const gl::Capability cap, const int index);
	static void disable(const gl::Capability cap);
	static void disable(const gl::Capability cap, const int index);
	static bool is_enabled(const gl::Capability cap);
	static bool is_enabled(const gl::Capability cap, const int index);

	static double get_time();
	static float get_scroll();

private:
	static void scroll_callback(GLFWwindow * const window, const double d_x, const double d_y);

	struct RendererConstructor {
		RendererConstructor();
		~RendererConstructor();

		float scroll_accumulator;
	};

	static RendererConstructor instance;
};

#endif
