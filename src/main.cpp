
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
#include "camera.hpp"

#include "perlin_noise.hpp"
#include "block.hpp"
#include "block_texture_atlas.hpp"


struct Player {
	Player() = default;
	void update(const float d_t, const Window &window);

	Camera camera;
};


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

static constexpr unsigned max_num_faces_in_chunk(const unsigned chunk_size) {
	const unsigned num_blocks = chunk_size * chunk_size * chunk_size / 2; // Num of blocks that maximize the number of faces to be drawn
	return num_blocks * (unsigned) FaceId::NUM_FACES;
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
	const Block::Id block,
	const FaceId face,
	std::vector<Vertex> &vertices)
{
	constexpr int num_vertices_per_face = 4;
	static const std::array<std::array<glm::ivec3, num_vertices_per_face>, (unsigned char) FaceId::NUM_FACES> face_positions = {{
		{{{1, 0, 0}, {1, 1, 0}, {0, 0, 0}, {0, 1, 0}}},
		{{{0, 0, 1}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1}}},
		{{{1, 0, 1}, {1, 1, 1}, {1, 0, 0}, {1, 1, 0}}},
		{{{0, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 1, 1}}},
		{{{0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}}},
		{{{0, 0, 0}, {0, 0, 1}, {1, 0, 0}, {1, 0, 1}}}
	}};


	const auto &main_text_position = Block::get_coords(block, face, FaceLayer::MAIN);
	const glm::vec2 vbo_main_text_positions[] = {
		{main_text_position.x_min, main_text_position.y_max},
		{main_text_position.x_min, main_text_position.y_min},
		{main_text_position.x_max, main_text_position.y_max},
		{main_text_position.x_max, main_text_position.y_min}
	};

	const auto &sec_text_position = Block::get_coords(block, face, FaceLayer::SECONDARY);
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
}


#include "world_generator.hpp"


struct ivec3_key : public glm::ivec3 {
	ivec3_key(const glm::ivec3 &vec);
	bool operator<(const ivec3_key &other) const;
};

ivec3_key::ivec3_key(const glm::ivec3 &vec) :
	glm::ivec3(vec) {}

bool ivec3_key::operator<(const ivec3_key &other) const {
	for(unsigned char i = 0; i < 3; i++)
		if((*this)[i] != other[i])
			return (*this)[i] < other[i];
	return false;
}


class Chunk;

class Chunks {
public:
	Chunks(const int radius);
	Block::Id operator[](const glm::ivec3 &world_pos) const;

	Chunk &get_chunk(const glm::ivec3 &chunk_pos);
	const Chunk &get_chunk(const glm::ivec3 &chunk_pos) const;

	void gen_chunks();
	void gen_chunks(const int quantity);
	void draw() const;
	void update();

private:
	std::map<ivec3_key, Chunk> chunks;
	std::queue<glm::ivec3> chunks_to_generate;
	WorldGenerator generator;
	int radius;
};


static const std::array<glm::ivec3, (unsigned char) FaceId::NUM_FACES> relative_pos = {{
	{ 0,  0, -1},
	{ 0,  0,  1},
	{ 1,  0,  0},
	{-1,  0,  0},
	{ 0,  1,  0},
	{ 0, -1,  0}
}};


class Chunk {
public:
	Chunk() = default;

	Chunk(const glm::ivec3 &position, WorldGenerator &generator) :
		built_buffer(false),
		drawable(false),
		need_update(true),
		position(position),
		buffer(layout),
		num_faces(0)
	{
		const glm::ivec2 pos2 = {this->position.x, this->position.z};
		for(int i = 0; i < chunk_size; i++) {
			for(int k = 0; k < chunk_size; k++) {
				const glm::ivec2 block_chunk_pos = {i, k};
				const glm::ivec2 block_global_pos = pos2 * chunk_size + block_chunk_pos;
				
				const int height = generator.get_height(block_global_pos);
				const int block_chunk_height = height - this->position.y * chunk_size;
				
				int j = 0;
				for(; j < (block_chunk_height - 3) && j < chunk_size; j++)
					this->blocks[i][j][k] = Block::Id::STONE;
				
				for(; j < block_chunk_height && j < chunk_size; j++)
					this->blocks[i][j][k] = Block::Id::DIRT;
				
				if(block_chunk_height >= 0 && block_chunk_height < chunk_size)
					this->blocks[i][j++][k] = Block::Id::GRASS;
				
				for(; j < chunk_size; j++)
					this->blocks[i][j][k] = Block::Id::AIR;
			}
		}
	}

	Block::Id operator[](const glm::ivec3 &position) const {
		if(
			position.x < 0 || position.x >= chunk_size ||
			position.y < 0 || position.y >= chunk_size ||
			position.z < 0 || position.z >= chunk_size
		)
			return Block::Id::NONE;
		return this->blocks[position.x][position.y][position.z];
	}

	void build_buffer_if_necessary(const Chunks &chunks) {
		
		if(this->built_buffer && !this->need_update)
			return;
		
		this->built_buffer = true;

		
		std::vector<Vertex> vertices;

		for(unsigned i = 0; i < chunk_size; i++) {
			for(unsigned j = 0; j < chunk_size; j++) {
				for(unsigned k = 0; k < chunk_size; k++) {
					const glm::ivec3 block_chunk_pos = {i, j, k};
					const glm::ivec3 block_world_pos = this->position * chunk_size + block_chunk_pos;
					
					const Block::Id block_id = (*this)[block_chunk_pos];
					const Block &block = Block::get_block(block_id);
					if(block.invisible)
						continue;

					for(unsigned char face_id = 0; face_id < (unsigned char) FaceId::NUM_FACES; face_id++) {
						const glm::ivec3 near_block_chunk_pos = block_chunk_pos + relative_pos[(unsigned char) face_id];
						Block::Id near_block_id = (*this)[near_block_chunk_pos];
						if(near_block_id == Block::Id::NONE) {
							const glm::ivec3 near_block_global_pos = this->position * chunk_size + near_block_chunk_pos;
							near_block_id = chunks[near_block_global_pos];

							if(near_block_id == Block::Id::NONE)
								continue;
						}
						
						const Block &near_block = Block::get_block(near_block_id);
						if(near_block.invisible)
							build_block_face(block_world_pos, block_id, (FaceId) face_id, vertices);
					}
				}
			}
		}


		if(vertices.size() == 0)
			return;
		
		this->buffer.assign_data<Vertex>(vertices, gl::Usage::STATIC_DRAW);
		
		this->need_update = false;
		this->drawable = true;
		this->num_faces = vertices.size() / num_vertices_per_face;
	}

	void mark_for_update() {
		this->need_update = true;
	}

	void draw() const {
		if(this->drawable) {
			this->buffer.bind();
			ibo.draw(this->num_faces * num_indices_per_face);
		}
	}
	
	static void intitate_ibo() {
		constexpr unsigned max_num_faces = max_num_faces_in_chunk(chunk_size);
		std::array<unsigned, max_num_faces * 6> indices;

		static const std::array<unsigned, 6> indices_base = {{
			2, 1, 0,
			2, 3, 1
		}};

		for(unsigned i = 0; i < max_num_faces; i++)
			for(unsigned j = 0; j < 6; j++)
				indices[i * 6 + j] = i * 4 + indices_base[j];

		new(&ibo) IndexBuffer<unsigned>(indices);
	}

private:
	static const std::array<LayoutElement, 5> layout;

	bool built_buffer: 1;
	bool drawable: 1;
	bool need_update: 1;
	Block::Id blocks[chunk_size][chunk_size][chunk_size];
	glm::ivec3 position;

	SuperBuffer buffer;
	unsigned num_faces;
	static IndexBuffer<unsigned> ibo;
};

const std::array<LayoutElement, 5> Chunk::layout = {{
	{3, GL_FLOAT, false},
	{3, GL_FLOAT, false},
	{2, GL_FLOAT, false},
	{2, GL_FLOAT, false},
	{1, GL_FLOAT, false}
}};

IndexBuffer<unsigned> Chunk::ibo;




Chunks::Chunks(const int radius) :
	radius(radius)
{
	if(this->radius >= 0)
		this->chunks_to_generate.push({0, 0, 0});

	for(int distance = 1; distance <= this->radius; distance++) {
		for(int i = -distance; i <= distance; i++) {
			for(int j = -distance; j <= distance; j++) {
				const glm::ivec3 pos = {i, distance, j};
				this->chunks_to_generate.push(pos);
				this->chunks_to_generate.push(-pos);
			}
		}

		for(int i = -distance; i <= distance; i++) {
			for(int j = -distance + 1; j < distance; j++) {
				const glm::ivec3 pos = {i, j, distance};
				this->chunks_to_generate.push(pos);
				this->chunks_to_generate.push(-pos);
			}
		}

		for(int i = -distance + 1; i < distance; i++) {
			for(int j = -distance + 1; j < distance; j++) {
				const glm::ivec3 pos = {distance, i, j};
				this->chunks_to_generate.push(pos);
				this->chunks_to_generate.push(-pos);
			}
		}
	}
}

Chunk &Chunks::get_chunk(const glm::ivec3 &chunk_pos) {
	const auto it = this->chunks.find(chunk_pos);
	assert(it != this->chunks.end());
	return it->second;
}

const Chunk &Chunks::get_chunk(const glm::ivec3 &chunk_pos) const {
	const auto it = this->chunks.find(chunk_pos);
	assert(it != this->chunks.end());
	return it->second;
}

void Chunks::gen_chunks() {
	while(!this->chunks_to_generate.empty()) {
		const auto pos = this->chunks_to_generate.front();
		this->chunks_to_generate.pop();
		
		Chunk &chunk = this->chunks[pos];		
		new (&chunk) Chunk(pos, this->generator);
		
		for(unsigned char i = 0; i < (unsigned char) FaceId::NUM_FACES; i++) {
			const glm::ivec3 side_chunk_pos = pos + relative_pos[i];
			const auto it = this->chunks.find(side_chunk_pos);
			if(it != this->chunks.end())
				it->second.mark_for_update();	
		}
	}
}

void Chunks::gen_chunks(const int quantity) {
	for(int i = 0; i < quantity; i++) {
		if(this->chunks_to_generate.empty())
			return;
		
		const auto pos = this->chunks_to_generate.front();
		this->chunks_to_generate.pop();

		Chunk &chunk = this->chunks[pos];		
		new (&chunk) Chunk(pos, this->generator);
		
		for(unsigned char i = 0; i < (unsigned char) FaceId::NUM_FACES; i++) {
			const glm::ivec3 side_chunk_pos = pos + relative_pos[i];
			const auto it = this->chunks.find(side_chunk_pos);
			if(it != this->chunks.end())
				it->second.mark_for_update();	
		}
	}
}


Block::Id Chunks::operator[](const glm::ivec3 &block_world_pos) const {
	const glm::ivec3 chunk_pos = get_chunk_pos_based_on_block_inside(block_world_pos);
	
	const auto it = this->chunks.find(chunk_pos);
	if(it == this->chunks.end())
		return Block::Id::NONE;
	
	const glm::ivec3 chunk_block_pos = block_world_pos - chunk_pos * chunk_size;
	return it->second[chunk_block_pos];
}

void Chunks::draw() const {
	for(const auto &chunk : this->chunks)
		chunk.second.draw();
}

void Chunks::update() {
	for(auto &chunk : this->chunks)
		chunk.second.build_buffer_if_necessary(*this);
}


int main() {

	#ifndef NDEBUG
	std::cout << std::fixed << std::setprecision(2);
	#endif


	auto window = Renderer::create_window({800, 600}, "Base");
	window.set_cursor_mode(gl::CursorMode::DISABLED);

	Renderer::enable(gl::Capability::CULL_FACE);
	Renderer::enable(gl::Capability::DEPTH_TEST);

	Chunk::intitate_ibo();


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

		chunks.draw();

		window.swap_buffers();
		Renderer::poll_events();
	}

	return 0;
}
