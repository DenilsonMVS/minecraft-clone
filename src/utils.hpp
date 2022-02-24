
#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>



#ifndef NDEBUG
#define PASS() pass_func(__LINE__, __FILE__, __func__)
void pass_func(const size_t line, const char * const file, const char * const function);
#else
#define PASS()
#endif


namespace det {

	void check(const bool status, const char * const message = "");

}


namespace det_gl {

	template<typename T>
	constexpr unsigned get_gl_enum() {
		//static_assert(false);
		return 0;
	}

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
}


#endif
