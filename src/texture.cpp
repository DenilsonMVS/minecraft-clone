
#include "texture.hpp"

#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <cassert>


Texture::Texture(const char * const path, const Texture::Filtering filtering, const Texture::Wrapping wrapping) {

	int width, height, channels;
	unsigned char * const buffer = stbi_load(path, &width, &height, &channels, STBI_default);
	assert(buffer != NULL);

	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_2D, this->id);
	if(channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

	stbi_image_free(buffer);
}

Texture::Texture(const unsigned char *image, const int width, const int height, const int channels, const Filtering filtering, const Wrapping wrapping) {
	glGenTextures(1, &this->id);

	glBindTexture(GL_TEXTURE_2D, this->id);
	if(channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
}

Texture::Texture(Texture &&other) :
	id(other.id)
{
	other.id = 0;
}

Texture &Texture::operator=(Texture &&other) {
	if(this != &other) {
		glDeleteTextures(1, &this->id);
		this->id = other.id;
		other.id = 0;
	}
	return (*this);
}

Texture::~Texture() {
	glDeleteTextures(1, &this->id);
}

void Texture::bind(const unsigned slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, this->id);
}

unsigned Texture::get_id() const {
	return this->id;
}
