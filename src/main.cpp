
#include <iostream>
#include <iomanip>

#include "block_texture_atlas.hpp"
#include "player.hpp"

#include "chunks.hpp"

#include "voxel_raycast.hpp"
#include "crosshair.hpp"
#include "block_selection.hpp"


static bool is_solid_block(const Chunks &chunks, const glm::ivec3 &position) {
	const auto block_id = chunks.get_block(position);
	if(block_id != Block::Id::NONE)
		return Block::get_block(block_id).solid;
	return false;
}

int main() {

	#ifndef NDEBUG
	std::cout << std::fixed << std::setprecision(2);
	#endif


	auto renderer = Renderer();

	
	auto window = renderer.create_window({800, 600}, "Base");
	window.set_cursor_mode(gl::CursorMode::DISABLED);

	renderer.set_max_drawn_quads(Chunk::max_faces_in_chunk());
	renderer.enable(gl::Capability::CULL_FACE);
	renderer.enable(gl::Capability::DEPTH_TEST);
	
	renderer.set_blend_function(gl::BlendFunc::SRC_ALPHA, gl::BlendFunc::ONE_MINUS_SRC_ALPHA);


	const auto block_selection = BlockSelection();
	const auto crosshair = Crosshair(3);

	const auto atlas = BlockTextureAtlas();

	auto player = Player();
	player.camera.position = {0, 0, 0};
	player.camera.front = {1, 0, 0};
	player.speed = 10;
	player.camera.sensitivity = 3;


	auto chunks = Chunks(3, player.position);


	float last_scroll = renderer.get_scroll();
	float last_time = renderer.get_time();
	renderer.set_clear_color(0.1, 0.05, 0.25);
	while(!window.should_close() && !(window.get_key_status(gl::Key::ESCAPE) == gl::KeyStatus::PRESS)) {
		
		const float current_time = renderer.get_time();
		const float d_t = current_time - last_time;
		last_time = current_time;

		const float current_scroll = renderer.get_scroll();
		const float rotation = current_scroll - last_scroll;
		last_scroll = current_scroll;


		player.speed *= 1 + rotation / 10;
		player.update(d_t, window);
		
		chunks.gen_chunks(1);

		const auto ray_location = cast_ray(chunks, player.position, player.camera.front, Player::range);
		if(ray_location) {
			if(window.get_mouse_button_status(gl::MouseButton::LEFT) == gl::KeyStatus::PRESS)
				chunks.modify_block(ray_location.value().back(), Block::Id::AIR);
			
			if(window.get_mouse_button_status(gl::MouseButton::RIGHT) == gl::KeyStatus::PRESS) {
				if(ray_location.value().size() >= 2) {
					const glm::ivec3 &position_to_place_block = ray_location.value()[ray_location.value().size() - 2];
					if(position_to_place_block != det::to_int(player.position))
						chunks.modify_block(position_to_place_block, Block::Id::GLASS);
				}
			}
		}
		
		chunks.update(player.position);
		

		renderer.clear(gl::BitField::COLOR_BUFFER | gl::BitField::DEPTH_BUFFER);

		#ifndef NDEBUG
		const glm::ivec3 int_pos = det::to_int(player.position);
		const glm::ivec3 chunk_pos = get_chunk_pos_based_on_block_inside(int_pos);
		std::cout << "pos: " << int_pos.x << ' ' << int_pos.y << ' ' << int_pos.z << '\n';
		std::cout << "chunk_pos: " << chunk_pos.x << ' ' << chunk_pos.y << ' ' << chunk_pos.z << '\n';
		#endif

		const auto mvp = player.camera.get_view_projection(window.get_dimensions(), 90);

		chunks.draw(mvp, renderer);

		if(ray_location && !is_solid_block(chunks, det::to_int(player.position)))
			block_selection.draw(ray_location.value().back(), mvp, renderer, player.position);
		
		crosshair.draw(window);


		window.swap_buffers();
		renderer.poll_events();
	}

	return 0;
}
