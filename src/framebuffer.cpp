
#include "framebuffer.hpp"


Framebuffer::Framebuffer(const glm::ivec2 &dimensions) {
	assert(dimensions.x >= 0);
	assert(dimensions.y >= 0);
	
	glGenFramebuffers(1, &this->fbo_id);
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo_id);

	glGenTextures(2, &this->depth_map);

	glBindTexture(GL_TEXTURE_2D, this->depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, dimensions.x, dimensions.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depth_map, 0);

	glBindTexture(GL_TEXTURE_2D, this->color_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->color_map, 0);
	
	det::check(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::Framebuffer(Framebuffer &&other) :
	fbo_id(other.fbo_id),
	depth_map(other.depth_map),
	color_map(other.color_map)
{
	other.fbo_id = 0;
	other.depth_map = 0;
	other.color_map = 0;
}

Framebuffer &Framebuffer::operator=(Framebuffer &&other) {
	if(this != &other) {
		glDeleteFramebuffers(1, &this->fbo_id);
		glDeleteTextures(2, &this->depth_map);

		this->fbo_id = other.fbo_id;
		this->depth_map = other.depth_map;
		this->color_map = other.color_map;

		other.fbo_id = 0;
		other.depth_map = 0;
		other.color_map = 0;
	}

	return *this;
}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &this->fbo_id);
	glDeleteTextures(2, &this->depth_map);
}

void Framebuffer::bind_color_map(const unsigned slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, this->color_map);
}

void Framebuffer::bind_depth_map(const unsigned slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, this->depth_map);
}

void Framebuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo_id);
}

void Framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
