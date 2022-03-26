
#ifndef UNIFORM_HPP
#define UNIFORM_HPP

#include "glm/glm.hpp"


class Uniform {
public:
	Uniform();
	Uniform(const int location);

	void set(const float value) const;
	void set(const glm::vec2 &value) const;
	void set(const glm::vec3 &value) const;
	void set(const glm::vec4 &value) const;

	void set(const int value) const;
	void set(const glm::ivec2 &value) const;
	void set(const glm::ivec3 &value) const;
	void set(const glm::ivec4 &value) const;

	void set(const glm::mat4 &value) const;

private:
	int location;
};

#endif
