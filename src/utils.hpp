
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


#endif
