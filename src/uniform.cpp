
#include "uniform.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"


Uniform::Uniform() :
	location(-1) {}

Uniform::Uniform(const int location) :
	location(location)
{
	assert(this->location != -1);
}

void Uniform::set(const float value) const {
	glUniform1f(this->location, value);
}

void Uniform::set(const glm::vec2 &value) const {
	glUniform2f(this->location, value[0], value[1]);
}

void Uniform::set(const glm::vec3 &value) const {
	glUniform3f(this->location, value[0], value[1], value[2]);
}

void Uniform::set(const glm::vec4 &value) const {
	glUniform4f(this->location, value[0], value[1], value[2], value[3]);
}

void Uniform::set(const int value) const {
	glUniform1i(this->location, value);
}

void Uniform::set(const glm::ivec2 &value) const {
	glUniform2i(this->location, value[0], value[1]);
}

void Uniform::set(const glm::ivec3 &value) const {
	glUniform3i(this->location, value[0], value[1], value[2]);
}

void Uniform::set(const glm::ivec4 &value) const {
	glUniform4i(this->location, value[0], value[1], value[2], value[3]);
}

void Uniform::set(const glm::mat4 &value) const {
	glUniformMatrix4fv(this->location, 1, GL_FALSE, glm::value_ptr(value));
}
