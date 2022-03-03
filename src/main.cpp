
#include <iostream>

#include "renderer.hpp"
#include "program.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "super_buffer.hpp"
#include "camera.hpp"


struct Player {
	Player(const Window &window);
	void update(const float d_t, const Window &window);

	Camera camera;
};

Player::Player(const Window &window) :
	camera(window.get_mouse_pos()) {}

void Player::update(const float d_t, const Window &window) {

	this->camera.update(window.get_mouse_pos(), d_t);


	const auto facing_zero_y_normalized = glm::normalize(glm::vec3(this->camera.front.x, 0.0f, this->camera.front.z));
	
	if(window.get_key_status(gl::Key::W) == gl::KeyStatus::PRESS)
		this->camera.position += this->camera.speed * facing_zero_y_normalized * d_t;
	
	if(window.get_key_status(gl::Key::S) == gl::KeyStatus::PRESS)
		this->camera.position -= this->camera.speed * facing_zero_y_normalized * d_t;
	
	if(window.get_key_status(gl::Key::A) == gl::KeyStatus::PRESS)
		this->camera.position -= this->camera.speed * glm::normalize(glm::cross(facing_zero_y_normalized, glm::vec3(0.0f, 1.0f, 0.0f))) * d_t;

	if(window.get_key_status(gl::Key::D) == gl::KeyStatus::PRESS)
		this->camera.position += this->camera.speed * glm::normalize(glm::cross(facing_zero_y_normalized, glm::vec3(0.0f, 1.0f, 0.0f))) * d_t;

	if(window.get_key_status(gl::Key::SPACE) == gl::KeyStatus::PRESS)
		this->camera.position.y += this->camera.speed * d_t;

	if(window.get_key_status(gl::Key::LEFT_SHIFT) == gl::KeyStatus::PRESS)
		this->camera.position.y -= this->camera.speed * d_t;
}



int main() {

	auto window = Renderer::create_window({800, 600}, "Base");
	window.set_cursor_mode(gl::CursorMode::DISABLED);

	auto player = Player(window);
	player.camera.position = {0, 0, -1};
	player.camera.front = {1, 0, 0};
	player.camera.speed = 3;
	player.camera.sensitivity = 3;


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
		indices, gl::Usage::STATIC_DRAW,
		MemoryHolder(vertices), gl::Usage::STATIC_DRAW,
		layout, 0);
	

	const auto texture = Texture("resources/images/dirt.png", Texture::NEAREST);

	const auto shaders = {
		Shader("resources/shaders/main.vert"),
		Shader("resources/shaders/main.frag")
	};

	const auto program = Program(shaders);
	auto u_texture = program.get_uniform("u_texture");
	u_texture.set(0);

	auto u_mvp = program.get_uniform("u_mvp");


	float last_time = Renderer::get_time();
	Renderer::set_clear_color(0.1, 0.05, 0.25);
	while(!window.should_close() && !(window.get_key_status(gl::Key::ESCAPE) == gl::KeyStatus::PRESS)) {
		Renderer::clear(gl::BitField::COLOR_BUFFER);


		const float current_time = Renderer::get_time();
		const float d_t = current_time - last_time;
		last_time = current_time;


		player.update(d_t, window);

		#ifndef NDEBUG
		std::cout << player.camera.position.x << ' ' << player.camera.position.y << player.camera.position.z << '\n';
		#endif

		u_mvp.set(player.camera.get_view_projection(window.get_dimensions(), 90));


		superbuffer.draw();

		window.swap_buffers();
		Renderer::poll_events();
	}

	return 0;
}
