
#include <iostream>
#include <iomanip>
#include <vector>

#include "renderer.hpp"
#include "program.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "super_buffer.hpp"
#include "camera.hpp"

#include "block.hpp"


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


struct Vertex {
	glm::vec3 position;
	glm::vec3 biome_color;
	glm::vec2 main_text_coord;
	glm::vec2 sec_text_coord;
	float bright;
};

static void build_block_face(
	const glm::ivec3 &position,
	BlockId block,
	const FaceId face,
	std::vector<Vertex> &vertices,
	std::vector<unsigned> &indices)
{
	constexpr int num_vertices_per_face = 4;
	static const glm::ivec3 face_positions[(unsigned char) FaceId::NUM_FACES][num_vertices_per_face] = {
		{{1, 0, 0}, {1, 1, 0}, {0, 0, 0}, {0, 1, 0}},
		{{0, 0, 1}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1}},
		{{1, 0, 1}, {1, 1, 1}, {1, 0, 0}, {1, 1, 0}},
		{{0, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 1, 1}},
		{{0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}},
		{{0, 0, 0}, {0, 0, 1}, {1, 0, 0}, {1, 0, 1}}
	};

	const auto &main_text_position = get_coords(block, face, FaceLayer::MAIN);
	const glm::vec2 vbo_main_text_positions[] = {
		{main_text_position.x_min, main_text_position.y_max},
		{main_text_position.x_min, main_text_position.y_min},
		{main_text_position.x_max, main_text_position.y_max},
		{main_text_position.x_max, main_text_position.y_min}
	};

	const auto &sec_text_position = get_coords(block, face, FaceLayer::SECONDARY);
	const glm::vec2 vbo_sec_text_positions[] = {
		{sec_text_position.x_min, sec_text_position.y_max},
		{sec_text_position.x_min, sec_text_position.y_min},
		{sec_text_position.x_max, sec_text_position.y_max},
		{sec_text_position.x_max, sec_text_position.y_min}
	};
	
	for(int i = 0; i < num_vertices_per_face; i++) {
		const Vertex vertex = {
			position + face_positions[(unsigned char) face][i],
			{0.1, 0.7, 0.15},
			vbo_main_text_positions[i],
			vbo_sec_text_positions[i],
			0.8};
		vertices.push_back(vertex);
	}

	
	static const unsigned indices_base[] = {
		2, 1, 0,
		2, 3, 1
	};
	const unsigned last_index = indices.size() / (sizeof(indices_base) / sizeof(indices_base[0])) * 4;
	for(const auto &index : indices_base)
		indices.push_back(last_index + index);
}


int main() {

	#ifndef NDEBUG
	std::cout << std::fixed << std::setprecision(2);
	#endif


	auto window = Renderer::create_window({800, 600}, "Base");
	window.set_cursor_mode(gl::CursorMode::DISABLED);

	Renderer::enable(gl::Capability::CULL_FACE);
	Renderer::enable(gl::Capability::DEPTH_TEST);


	auto atlas = BlockTextureAtlas();

	auto player = Player(window);
	player.camera.position = {0, 0, -1};
	player.camera.front = {1, 0, 0};
	player.camera.speed = 3;
	player.camera.sensitivity = 3;



	std::vector<unsigned> indices;
	std::vector<Vertex> vertices;

	build_block_face({0, 0, 0}, BlockId::GRASS, FaceId::NORTH, vertices, indices);
	build_block_face({0, 0, 0}, BlockId::GRASS, FaceId::SOUTH, vertices, indices);
	build_block_face({0, 0, 0}, BlockId::GRASS, FaceId::EAST, vertices, indices);
	build_block_face({0, 0, 0}, BlockId::GRASS, FaceId::WEST, vertices, indices);
	build_block_face({0, 0, 0}, BlockId::GRASS, FaceId::TOP, vertices, indices);
	build_block_face({0, 0, 0}, BlockId::GRASS, FaceId::BOTTOM, vertices, indices);

	const LayoutElement layout[] = {
		{3, GL_FLOAT, false},
		{3, GL_FLOAT, false},
		{2, GL_FLOAT, false},
		{2, GL_FLOAT, false},
		{1, GL_FLOAT, false}
	};

	auto superbuffer = SuperBuffer<unsigned>(
		indices, gl::Usage::STATIC_DRAW,
		vertices, gl::Usage::STATIC_DRAW,
		layout, 0);
	
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
		Renderer::clear(gl::BitField::COLOR_BUFFER | gl::BitField::DEPTH_BUFFER);


		const float current_time = Renderer::get_time();
		const float d_t = current_time - last_time;
		last_time = current_time;


		player.update(d_t, window);

		#ifndef NDEBUG
		std::cout << player.camera.position.x << ' ' << player.camera.position.y << ' ' << player.camera.position.z << '\n';
		#endif

		u_mvp.set(player.camera.get_view_projection(window.get_dimensions(), 90));


		superbuffer.draw();

		window.swap_buffers();
		Renderer::poll_events();
	}

	return 0;
}
