
#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "shader.hpp"
#include "uniform.hpp"
#include <span>


class Program {
public:
	Program();
	Program(const std::span<const Shader> &shaders);
	Program(const std::span<const char * const> &shader_sources);

	Program(Program &&other);
	Program &operator=(Program &&other);

	~Program();

	unsigned get_id() const;
	void bind() const;

	Uniform get_uniform(const char * const name) const;

private:
	unsigned id;
};

#endif
