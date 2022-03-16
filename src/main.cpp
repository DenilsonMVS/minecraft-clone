
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <queue>
#include <random>
#include <array>

#include "renderer.hpp"
#include "program.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "super_buffer.hpp"
#include "index_buffer.hpp"

#include "perlin_noise.hpp"
#include "block.hpp"
#include "block_texture_atlas.hpp"
#include "player.hpp"
#include "world_generator.hpp"

#include "chunk.hpp"
#include "chunks.hpp"


int main() {

	#ifndef NDEBUG
	std::cout << std::fixed << std::setprecision(2);
	#endif


	auto window = Renderer::create_window({800, 600}, "Base");
	window.set_cursor_mode(gl::CursorMode::DISABLED);

	Renderer::enable(gl::Capability::CULL_FACE);
	Renderer::enable(gl::Capability::DEPTH_TEST);

	auto face_ibo = BlockFace::gen_index_buffer_to_gen_faces(Chunk::max_faces_in_chunk());


	auto atlas = BlockTextureAtlas();

	auto player = Player();
	player.camera.position = {0, 0, 0};
	player.camera.front = {1, 0, 0};
	player.camera.speed = 10;
	player.camera.sensitivity = 3;


	auto chunks = Chunks(3);
	chunks.gen_chunks();
	chunks.update();


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
		std::cout << "pos: " << player.camera.position.x << ' ' << player.camera.position.y << ' ' << player.camera.position.z << '\n';
		const glm::ivec3 int_pos = player.camera.position;
		const glm::ivec3 chunk_pos = get_chunk_pos_based_on_block_inside(int_pos);
		std::cout << "chunk_pos: " << chunk_pos.x << ' ' << chunk_pos.y << ' ' << chunk_pos.z << '\n';
		#endif

		u_mvp.set(player.camera.get_view_projection(window.get_dimensions(), 90));

		chunks.draw(face_ibo);

		window.swap_buffers();
		Renderer::poll_events();
	}

	return 0;
}
