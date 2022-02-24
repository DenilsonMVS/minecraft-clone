
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"


class Renderer {
public:
	static Window create_window(const int width, const int height, const char * const title);
	static Window create_window(const int width, const int height, const char * const title, const Window &share);

	static void set_clear_color(const float r, const float g, const float b);
	static void set_clear_color(const glm::vec3 &color);
	static void poll_events();
	static void clear(const unsigned mask);

	template<typename T>
	static void draw(const VertexBuffer<T> &vbo) {
		glDrawArrays(GL_TRIANGLES, 0, vbo.get_count());
	}
	
	template<typename T>
	static void draw(const IndexBuffer<T> &ibo) {
		glDrawElements(GL_TRIANGLES, ibo.get_count(), ibo.get_type(), 0);
	}

private:
	struct RendererConstructor {
		RendererConstructor();
		~RendererConstructor();
	};

	static RendererConstructor instance;
};

#endif
