
#include "renderer.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"
#include "program.hpp"
#include "vertex_array.hpp"
#include "texture.hpp"


int main() {

	auto window = Renderer::create_window(800, 600, "Base");
	

	const auto texture = Texture("resources/images/dirt.png", Texture::NEAREST);

	const float vertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f, 1.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		 0.5f, -0.5f, 1.0f, 0.0f
	};
	const auto vbo = VertexBuffer<float>(vertices, GL_STATIC_DRAW);

	const uint8_t indices[] = {
		0, 1, 2,
		0, 2, 3
	};
	auto ibo = IndexBuffer<unsigned char>(indices, GL_STATIC_DRAW);
	

	const auto shaders = {
		Shader("resources/shaders/main.vert"),
		Shader("resources/shaders/main.frag")
	};

	const auto program = Program(shaders);
	
	auto layout = VertexBufferLayout();
	layout.push(2, GL_FLOAT, GL_FALSE);
	layout.push(2, GL_FLOAT, GL_FALSE);

	const auto vao = VertexArray(layout, vbo);


	auto u_texture = program.get_uniform("u_texture");
	u_texture.set(0);

	vao.bind();
	ibo.bind();
	texture.bind(0);


	Renderer::set_clear_color(0.1, 0.05, 0.25);
	while(!window.should_close()) {
		Renderer::clear(GL_COLOR_BUFFER_BIT);

		Renderer::draw(ibo);

		window.swap_buffers();
		Renderer::poll_events();
	}

	return 0;
}
