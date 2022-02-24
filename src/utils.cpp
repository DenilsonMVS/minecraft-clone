
#include "utils.hpp"

#include <iostream>


#ifndef NDEBUG
void pass_func(const size_t line, const char * const file, const char * const function) {
	std::cout << file << ':' << line << ':' << function << ": PASS\n";
}
#endif

namespace det {

	void check(const bool status, const char * const message) {
		if(!status) {
			std::cerr << message;
			exit(EXIT_FAILURE);
		}
	}

}
