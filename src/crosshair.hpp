
#ifndef CROSSHAIR_HPP
#define CROSSHAIR_HPP

#include <glm/glm.hpp>
#include <array>

#include "window.hpp"
#include "super_buffer.hpp"
#include "index_buffer.hpp"
#include "program.hpp"


class Crosshair {
public:
	Crosshair(const float size);

	void draw(const Window &window) const;

private:
	struct CrosshairVertex {
		glm::vec2 position;
	};

	static const std::array<LayoutElement, 1> layout;

	SuperBuffer buffer;
	IndexBuffer<unsigned char> ibo;
	Program program;
	Uniform u_width;
	Uniform u_height;
};

#endif
