
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.hpp"



namespace gl {

	template<typename T>
	constexpr unsigned get_gl_enum();

	template<>
	constexpr unsigned get_gl_enum<char>() {
		return GL_BYTE;
	}

	template<>
	constexpr unsigned get_gl_enum<unsigned char>() {
		return GL_UNSIGNED_BYTE;
	}

	template<>
	constexpr unsigned get_gl_enum<short>() {
		return GL_SHORT;
	}

	template<>
	constexpr unsigned get_gl_enum<unsigned short>() {
		return GL_UNSIGNED_SHORT;
	}

	template<>
	constexpr unsigned get_gl_enum<int>() {
		return GL_INT;
	}

	template<>
	constexpr unsigned get_gl_enum<unsigned int>() {
		return GL_UNSIGNED_INT;
	}

	template<>
	constexpr unsigned get_gl_enum<float>() {
		return GL_FLOAT;
	}

	template<>
	constexpr unsigned get_gl_enum<double>() {
		return GL_DOUBLE;
	}


	constexpr unsigned get_size(const unsigned type) {
		switch(type) {
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			return 1;

		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_HALF_FLOAT:
			return 2;

		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_FIXED:
		case GL_FLOAT:
			return 4;

		case GL_DOUBLE:
			return 8;

		default:
			assert(false);
			return 0;
		}
	}

	enum Usage : unsigned {
		STREAM_DRAW = GL_STREAM_DRAW,
		STREAM_READ = GL_STREAM_READ,
		STREAM_COPY = GL_STREAM_COPY,
		STATIC_DRAW = GL_STATIC_DRAW,
		STATIC_READ = GL_STATIC_READ,
		STATIC_COPY = GL_STATIC_COPY,
		DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
		DYNAMIC_READ = GL_DYNAMIC_READ,
		DYNAMIC_COPY = GL_DYNAMIC_COPY,
	};
}


class Renderer {
public:
	static Window create_window(const int width, const int height, const char * const title);
	static Window create_window(const int width, const int height, const char * const title, const Window &share);

	static void set_clear_color(const float r, const float g, const float b);
	static void set_clear_color(const glm::vec3 &color);
	static void poll_events();
	static void clear(const unsigned mask);

private:
	struct RendererConstructor {
		RendererConstructor();
		~RendererConstructor();
	};

	static RendererConstructor instance;
};

#endif
