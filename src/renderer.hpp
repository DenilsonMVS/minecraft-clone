
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include "window.hpp"
#include "index_buffer.hpp"


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

	void set_max_drawn_quads(const unsigned quantity);
	void draw_quads(const unsigned quantity, const gl::DrawMode mode = gl::DrawMode::TRIANGLES) const;

private:
	struct Initializer {
		Initializer();
		~Initializer();
	};

	static void scroll_callback(GLFWwindow * const window, const double d_x, const double d_y);

	static constexpr unsigned num_indices_per_face = 6;
    static constexpr unsigned num_vertices_per_face = 4;
	
	static float scroll_accumulator;

	// Initializer should be first for correct glfw and OpenGl initialization and finalization
	Initializer initializer_instance;
	IndexBuffer<unsigned> face_indices;
};

#endif
