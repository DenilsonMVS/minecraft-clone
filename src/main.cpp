
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <queue>
#include <random>
#include <array>
#include <optional>

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


static unsigned index_of_smaller(const float * const v, const unsigned count) {
	assert(v != nullptr);
	assert(count > 0);

	unsigned smaller = 0;
	for(unsigned i = 1; i < count; i++)
		if(v[i] < v[smaller])
			smaller = i;
	return smaller;
}

static std::optional<glm::ivec3> cast_ray(
	const Chunks &chunks,
	const glm::vec3 &position,
	const glm::vec3 &facing,
	const float radius)
{
	const float step = 0.1;
	const glm::vec3 step_vec = facing * step;
	const unsigned num_steps = radius / step;
	

	glm::vec3 current_pos = position;
	for(unsigned i = 0; i < num_steps; i++) {
		const glm::ivec3 block_global_pos = det::to_int(current_pos);
		const auto block_id = chunks.get_block(block_global_pos);

		if(block_id != Block::Id::NONE) {
			const auto &block = Block::get_block(block_id);
			if(!block.invisible)
				return block_global_pos;
		}
		
		current_pos += step_vec;
	}

	return std::nullopt;
}


class BlockSelection {
public:
	BlockSelection();

	void draw(const glm::ivec3 &block_position, const glm::mat4 &mvp, const Renderer &renderer) const;

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
	Uniform u_displacement;

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
	this->u_displacement = this->program.get_uniform("u_displacement");

	this->build_buffer();
}

void BlockSelection::draw(const glm::ivec3 &block_position, const glm::mat4 &mvp, const Renderer &renderer) const {
	constexpr unsigned num_faces_in_cube = 6;
	
	renderer.enable(gl::Capability::BLEND);

	this->program.bind();
	this->u_mvp.set(mvp);
	this->u_displacement.set(glm::vec3(block_position));

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
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	auto block_selection = BlockSelection();

	auto atlas = BlockTextureAtlas();

	auto player = Player();
	player.camera.position = {0, 0, 0};
	player.camera.front = {1, 0, 0};
	player.camera.speed = 10;
	player.camera.sensitivity = 3;


	auto chunks = Chunks(3, player.camera.position);


	float last_scroll = renderer.get_scroll();
	float last_time = renderer.get_time();
	renderer.set_clear_color(0.1, 0.05, 0.25);
	while(!window.should_close() && !(window.get_key_status(gl::Key::ESCAPE) == gl::KeyStatus::PRESS)) {
		chunks.gen_chunks(1);
		chunks.update(player.camera.position);

		renderer.clear(gl::BitField::COLOR_BUFFER | gl::BitField::DEPTH_BUFFER);

		const auto ray_location = cast_ray(chunks, player.camera.position, player.camera.front, Player::range);
		if(ray_location && window.get_mouse_button_status(gl::MouseButton::LEFT) == gl::KeyStatus::PRESS)	
			chunks.modify_block(ray_location.value(), Block::Id::AIR);


		const float current_time = renderer.get_time();
		const float d_t = current_time - last_time;
		last_time = current_time;

		const float current_scroll = renderer.get_scroll();
		const float rotation = current_scroll - last_scroll;
		last_scroll = current_scroll;


		player.camera.speed *= 1 + rotation / 10;
		player.update(d_t, window);

		#ifndef NDEBUG
		const glm::ivec3 int_pos = det::to_int(player.camera.position);
		const glm::ivec3 chunk_pos = get_chunk_pos_based_on_block_inside(int_pos);
		std::cout << "pos: " << int_pos.x << ' ' << int_pos.y << ' ' << int_pos.z << '\n';
		std::cout << "chunk_pos: " << chunk_pos.x << ' ' << chunk_pos.y << ' ' << chunk_pos.z << '\n';
		#endif

		const auto mvp = player.camera.get_view_projection(window.get_dimensions(), 90);

		chunks.draw(mvp, renderer);

		if(ray_location)
			block_selection.draw(ray_location.value(), mvp, renderer);


		window.swap_buffers();
		renderer.poll_events();
	}

	return 0;
}
