
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <queue>
#include <random>
#include <array>
#include <optional>
#include <fixed.hpp>

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

#include "voxel_raycast.hpp"


static bool is_air_block(const Chunks &chunks, const glm::ivec3 &position) {
	return chunks.get_block(position) == Block::Id::AIR;
}

class BlockSelection {
public:
	BlockSelection();

	void draw(const glm::ivec3 &block_position, const glm::mat4 &mvp, const Renderer &renderer, const glm::dvec3 &player_pos) const;

private:
	struct SelectionFaceVertex {
		glm::vec3 position;
		glm::vec2 text_coord;
	};

	void build_buffer();
	static void append_face_vertices(const FaceId face_id, std::vector<SelectionFaceVertex> &vertices);
	static glm::vec3 apply_eps_to_vec(const glm::vec3 &v);

	SuperBuffer buffer;
	Program program;
	Uniform u_mvp;
	Uniform u_offset;

	static const float eps;
	static const std::array<LayoutElement, 2> layout;
};

BlockSelection::BlockSelection() :
	buffer(layout)
{
	const Shader shaders[] = {
		Shader("resources/shaders/block_slection.vert"),
		Shader("resources/shaders/block_slection.frag")
	};

	new (&this->program) Program(shaders);
	const auto u_texture = program.get_uniform("u_texture");
	u_texture.set(0);
	this->u_mvp = this->program.get_uniform("u_mvp");
	this->u_offset = this->program.get_uniform("u_offset");

	this->build_buffer();
}

void BlockSelection::draw(const glm::ivec3 &block_position, const glm::mat4 &mvp, const Renderer &renderer, const glm::dvec3 &player_pos) const {
	constexpr unsigned num_faces_in_cube = 6;

	renderer.enable(gl::Capability::BLEND);

	const glm::ivec3 center_chunk_pos = get_chunk_pos_based_on_block_inside(det::to_int(player_pos));
	const glm::ivec3 block_chunk_pos = get_chunk_pos_based_on_block_inside(block_position);
	const glm::ivec3 relative_chunk_position = block_chunk_pos - center_chunk_pos;
	const glm::ivec3 block_position_inside_chunk = block_position - block_chunk_pos * chunk_size;

	this->program.bind();
	this->u_mvp.set(mvp);
	this->u_offset.set(glm::vec3(relative_chunk_position * chunk_size + block_position_inside_chunk));

	this->buffer.bind();
	renderer.draw_quads(num_faces_in_cube);
}

void BlockSelection::build_buffer() {
	constexpr unsigned num_faces_in_cube = 6;
	
	std::vector<SelectionFaceVertex> vertices;
	vertices.reserve(num_faces_in_cube * num_vertices_per_face);

	for(unsigned char i = 0; i < (unsigned char) FaceId::NUM_FACES; i++)
		this->append_face_vertices((FaceId) i, vertices);
	
	this->buffer.assign_data<SelectionFaceVertex>(vertices, gl::Usage::STATIC_DRAW);
}

void BlockSelection::append_face_vertices(const FaceId face_id, std::vector<BlockSelection::SelectionFaceVertex> &vertices) {
	
	const auto &texture_coords = BlockFace::get_block_face(BlockFace::Id::SELECTION).coords;
	const glm::vec2 texture_position[] = {
		{texture_coords.x_min, texture_coords.y_max},
		{texture_coords.x_min, texture_coords.y_min},
		{texture_coords.x_max, texture_coords.y_max},
		{texture_coords.x_max, texture_coords.y_min}
	};

	for(unsigned char i = 0; i < num_vertices_per_face; i++) {
		const SelectionFaceVertex vertex = {
			apply_eps_to_vec(BlockFace::face_positions_helper_data[(unsigned char) face_id][i]),
			texture_position[i]
		};
		vertices.push_back(vertex);
	}
}

glm::vec3 BlockSelection::apply_eps_to_vec(const glm::vec3 &v) {
	return {
		v.x > 0.5 ? v.x + eps : v.x - eps,
		v.y > 0.5 ? v.y + eps : v.y - eps,
		v.z > 0.5 ? v.z + eps : v.z - eps
	};
}

const float BlockSelection::eps = 0.001;

const std::array<LayoutElement, 2> BlockSelection::layout = {{
	{3, gl::get_enum<float>(), false},
	{2, gl::get_enum<float>(), false}
}};


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
		chunks.gen_chunks(1);
		chunks.update(player.position);

		renderer.clear(gl::BitField::COLOR_BUFFER | gl::BitField::DEPTH_BUFFER);

		const auto ray_location = cast_ray(chunks, player.position, player.camera.front, Player::range);
		if(ray_location && window.get_mouse_button_status(gl::MouseButton::LEFT) == gl::KeyStatus::PRESS)	
			chunks.modify_block(ray_location.value(), Block::Id::AIR);


		const float current_time = renderer.get_time();
		const float d_t = current_time - last_time;
		last_time = current_time;

		const float current_scroll = renderer.get_scroll();
		const float rotation = current_scroll - last_scroll;
		last_scroll = current_scroll;


		player.speed *= 1 + rotation / 10;
		player.update(d_t, window);

		#ifndef NDEBUG
		const glm::ivec3 int_pos = det::to_int(player.position);
		const glm::ivec3 chunk_pos = get_chunk_pos_based_on_block_inside(int_pos);
		std::cout << "pos: " << int_pos.x << ' ' << int_pos.y << ' ' << int_pos.z << '\n';
		std::cout << "chunk_pos: " << chunk_pos.x << ' ' << chunk_pos.y << ' ' << chunk_pos.z << '\n';
		#endif

		const auto mvp = player.camera.get_view_projection(window.get_dimensions(), 90);

		chunks.draw(mvp, renderer, player.position);

		if(ray_location && is_air_block(chunks, det::to_int(player.position)))
			block_selection.draw(ray_location.value(), mvp, renderer, player.position);
		
		crosshair.draw(window);


		window.swap_buffers();
		renderer.poll_events();
	}

	return 0;
}
