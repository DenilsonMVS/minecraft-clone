
#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>


class Texture {
public:

	enum Filtering {
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR
	};
	
	enum Wrapping {
		REPEAT = GL_REPEAT,
		MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
		CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
		CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER
	};


	Texture(const char * const path, const Filtering filtering = LINEAR, const Wrapping wrapping = CLAMP_TO_BORDER);

	Texture(Texture &&other);
	Texture &operator=(Texture &&other);

	Texture(const Texture &other) = delete;
	Texture &operator=(const Texture &other) = delete;

	~Texture();

	void bind(const unsigned slot) const;

	unsigned get_id() const;

private:
	unsigned id;
};

#endif
