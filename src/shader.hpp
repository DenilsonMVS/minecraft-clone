
#ifndef SHADER_HPP
#define SHADER_HPP


class Shader {
public:
	Shader();
	Shader(const char * const path); // Detects if is a fragment shader or vertex shader based on file extension, .frag and .vert
	Shader(const char * const path, const unsigned type);
	

	Shader(Shader &&other);
	Shader &operator=(Shader &&other);

	~Shader();

	unsigned get_id() const;

private:
	unsigned id;
};

#endif
