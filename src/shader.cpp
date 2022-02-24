
#include "shader.hpp"

#include <cstring>
#include <cassert>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


static std::string string_from_file(const char * const path) {
	auto ss = std::stringstream();
	auto ifs = std::ifstream(path);

	#ifndef NDEBUG
	if(!ifs.is_open()) {
		std::cerr << "The file or directory \"" << path << "\" does not exist.\n";
		exit(EXIT_FAILURE);
	}
	#endif

	ss << ifs.rdbuf();

	return ss.str();
}

static bool have_subsequence_at_end(const char * const str, const char * const end_subsequence) {
	const auto str_len = strlen(str);
	const auto end_len = strlen(end_subsequence);

	assert(str_len >= end_len);
	return memcmp(&str[str_len - end_len], end_subsequence, end_len) == 0;
}

static void check_shader_compiling_errors(const Shader &shader) {
	int compile_status;
	glGetShaderiv(shader.get_id(), GL_COMPILE_STATUS, &compile_status);
	if(compile_status == GL_FALSE) {

		int length;
		glGetShaderiv(shader.get_id(), GL_INFO_LOG_LENGTH, &length);

		char message[length + 1];
		glGetShaderInfoLog(shader.get_id(), length, &length, message);
		message[length] = '\0';

		std::cerr << "Failed to compile shader. " << message;
	}
}


Shader::Shader() {}

Shader::Shader(const char * const path) {

	const auto src = string_from_file(path);
	const auto c_src = src.c_str();
	
	if(have_subsequence_at_end(path, ".vert"))
		this->id = glCreateShader(GL_VERTEX_SHADER);
	else
		this->id = glCreateShader(GL_FRAGMENT_SHADER);
	
	glShaderSource(this->id, 1, &c_src, nullptr);
	glCompileShader(this->id);

	#ifndef NDEBUG
	check_shader_compiling_errors(*this);
	#endif
}

Shader::Shader(const char * const path, const unsigned type) {

	const auto src = string_from_file(path);
	const auto c_src = src.c_str();

	this->id = glCreateShader(type);
	glShaderSource(this->id, 1, &c_src, nullptr);
	glCompileShader(this->id);

	
	#ifndef NDEBUG
	check_shader_compiling_errors(*this);
	#endif
}

Shader::Shader(Shader &&other) :
	id(other.id)
{
	other.id = 0;
}

Shader &Shader::operator=(Shader &&other) {
	if(this != &other) {
		glDeleteShader(this->id);
		this->id = other.id;
		other.id = 0;
	}
	return (*this);
}

Shader::~Shader() {
	glDeleteShader(this->id);
}

unsigned Shader::get_id() const {
	return this->id;
}
