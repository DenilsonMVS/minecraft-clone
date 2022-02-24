
#include "program.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"


Program::Program() :
	id(0) {}

Program::Program(const std::span<const Shader> &shaders) :
	id(glCreateProgram())
{

	for(const auto &shader : shaders)
		glAttachShader(id, shader.get_id());

	glLinkProgram(this->id);


	#ifndef NDEBUG

	int status;
	glGetProgramiv(this->id, GL_LINK_STATUS, &status);
	if(status == GL_FALSE) {
		int length;
		glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &length);

		char message[length + 1];
		glGetProgramInfoLog(this->id, length, &length, message);
		message[length] = '\0';

		std::cerr << "Failed to link program. " << message;
	}

	#endif
}

Program::Program(Program &&other) :
	id(other.id) {
	other.id = 0;
}

Program &Program::operator=(Program &&other) {
	if(this != &other) {
		glDeleteProgram(this->id);
		this->id = other.id;
		other.id = 0;
	}
	return *this;
}

Program::~Program() {
	glDeleteProgram(this->id);
}

unsigned Program::get_id() const {
	return this->id;
}

void Program::bind() const {
	glUseProgram(this->id);
}

Uniform Program::get_uniform(const char * const name) const {
	glUseProgram(this->id);
	return Uniform(glGetUniformLocation(this->id, name));
}
