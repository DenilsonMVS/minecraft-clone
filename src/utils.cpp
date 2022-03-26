
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

	
	glm::ivec3 to_int(const glm::vec3 &v) {
		return glm::ivec3 {
			std::floor(v.x),
			std::floor(v.y),
			std::floor(v.z)
		};
	}

	glm::ivec3 to_int(const glm::dvec3 &v) {
		return glm::ivec3 {
			std::floor(v.x),
			std::floor(v.y),
			std::floor(v.z)
		};
	}

	int infinite_norm(const glm::ivec3 &v) {
		return std::max(std::abs(v[0]), std::max(std::abs(v[1]), std::abs(v[2])));
	}

	int linear_norm(const glm::ivec3 v) {
		return std::abs(v[0]) + std::abs(v[1]) + std::abs(v[2]);
	}

}
