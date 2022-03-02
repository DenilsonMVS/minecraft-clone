
#include "renderer.hpp"
#include "program.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "super_buffer.hpp"


int main() {

	auto window = Renderer::create_window(800, 600, "Base");
	

	const uint8_t indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	const float vertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f, 1.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		 0.5f, -0.5f, 1.0f, 0.0f
	};

	const LayoutElement layout[] = {
		{2, GL_FLOAT, false},
		{2, GL_FLOAT, false}
	};

	auto superbuffer = SuperBuffer<unsigned char>(
		indices, gl::STATIC_DRAW,
		MemoryHolder(vertices), gl::STATIC_DRAW,
		layout, 0);
	

	const auto texture = Texture("resources/images/dirt.png", Texture::NEAREST);

	const auto shaders = {
		Shader("resources/shaders/main.vert"),
		Shader("resources/shaders/main.frag")
	};

	const auto program = Program(shaders);
	auto u_texture = program.get_uniform("u_texture");
	u_texture.set(0);


	Renderer::set_clear_color(0.1, 0.05, 0.25);
	while(!window.should_close()) {
		Renderer::clear(gl::COLOR_BUFFER_BIT);

		superbuffer.draw();

		window.swap_buffers();
		Renderer::poll_events();
	}

	return 0;
}
