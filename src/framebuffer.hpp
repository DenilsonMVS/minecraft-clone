
#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <glm/glm.hpp>
#include "utils.hpp"


class Framebuffer {
public:
	Framebuffer() = default;
	Framebuffer(const glm::ivec2 &dimensions);

	Framebuffer(const Framebuffer &other) = delete;
	Framebuffer &operator=(const Framebuffer &other) = delete;

	Framebuffer(Framebuffer &&other);
	Framebuffer &operator=(Framebuffer &&other);

	void resize(const glm::ivec2 &dimensions);

	~Framebuffer();


	void bind_color_map(const unsigned slot) const;
	void bind_depth_map(const unsigned slot) const;
	void bind() const;

	static void unbind();


private:
	glm::ivec2 dimensions;
	unsigned fbo_id;
	unsigned depth_map;
	unsigned color_map;
};


#endif
