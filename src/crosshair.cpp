
#include "crosshair.hpp"


Crosshair::Crosshair(const float size) :
	buffer(layout)
{
	constexpr float thickness = 1;
	constexpr float height = 6;
	const std::array<CrosshairVertex, 12> vertices = {{
		{{-thickness * size, -height * size}},
		{{-thickness * size,  height * size}},
		{{ thickness * size,  height * size}},
		{{ thickness * size, -height * size}},
		
		{{-height * size,  thickness * size}},
		{{-thickness * size,  thickness * size}},
		{{-thickness * size, -thickness * size}},
		{{-height * size, -thickness * size}},

		{{height * size,  thickness * size}},
		{{thickness * size,  thickness * size}},
		{{thickness * size, -thickness * size}},
		{{height * size, -thickness * size}}
	}};

	this->buffer.assign_data<CrosshairVertex>(vertices, gl::Usage::STATIC_DRAW);


	const std::array<unsigned char, 18> indices = {{
		 1,  0,  2,
		 2,  0,  3,

		 5,  4,  6,
		 6,  4,  7,

		 8,  9, 10,
		 8, 10, 11
	}};
	new (&this->ibo) IndexBuffer<unsigned char>(indices);


	const std::array<Shader, 2> shaders = {{
		Shader("resources/shaders/crosshair.vert"),
		Shader("resources/shaders/crosshair.frag")
	}};
	new (&this->program) Program(shaders);

	this->u_width = this->program.get_uniform("u_width");
	this->u_height = this->program.get_uniform("u_height");
}

void Crosshair::draw(const Window &window) const {
	const glm::ivec2 window_dimensions = window.get_dimensions();
	this->program.bind();
	this->u_width.set(window_dimensions.x);
	this->u_height.set(window_dimensions.y);

	this->buffer.bind();
	this->ibo.draw();
}

const std::array<LayoutElement, 1> Crosshair::layout = {{
	{2, gl::get_enum<float>(), false}
}};
