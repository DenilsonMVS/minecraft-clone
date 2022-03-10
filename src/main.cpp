
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
	static const std::array<std::array<glm::ivec3, num_vertices_per_face>, (unsigned char) FaceId::NUM_FACES> face_positions = {{
		{{{1, 0, 0}, {1, 1, 0}, {0, 0, 0}, {0, 1, 0}}},
		{{{0, 0, 1}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1}}},
		{{{1, 0, 1}, {1, 1, 1}, {1, 0, 0}, {1, 1, 0}}},
		{{{0, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 1, 1}}},
		{{{0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}}},
		{{{0, 0, 0}, {0, 0, 1}, {1, 0, 0}, {1, 0, 1}}}
	}};


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

	static const std::array<unsigned, 6> indices_base = {{
		2, 1, 0,
		2, 3, 1
	}};
	const unsigned last_index = indices.size() / indices_base.size() * 4;
	for(const auto &index : indices_base)
		indices.push_back(last_index + index);
}


constexpr int chunk_size = 32;

static glm::ivec3 get_chunk_pos_based_on_block_inside(const glm::ivec3 &block_global_pos) {
	return {
		block_global_pos.x < 0 ? (block_global_pos.x + 1) / chunk_size - 1 : block_global_pos.x / chunk_size,
		block_global_pos.y < 0 ? (block_global_pos.y + 1) / chunk_size - 1 : block_global_pos.y / chunk_size,
		block_global_pos.z < 0 ? (block_global_pos.z + 1) / chunk_size - 1 : block_global_pos.z / chunk_size
	};
}

static glm::ivec2 get_chunk_pos_based_on_block_inside(const glm::ivec2 &block_global_pos) {
	return {
		block_global_pos.x < 0 ? (block_global_pos.x + 1) / chunk_size - 1 : block_global_pos.x / chunk_size,
		block_global_pos.y < 0 ? (block_global_pos.y + 1) / chunk_size - 1 : block_global_pos.y / chunk_size
	};
}


struct ivec2_key : public glm::ivec2 {
	ivec2_key(const glm::ivec2 &vec);
	bool operator<(const ivec2_key &other) const;
};

ivec2_key::ivec2_key(const glm::ivec2 &vec) :
	glm::ivec2(vec) {}

bool ivec2_key::operator<(const ivec2_key &other) const {
	for(unsigned char i = 0; i < 2; i++)
		if((*this)[i] != other[i])
			return (*this)[i] < other[i];
	return false;
}

struct WorldGeneratorChunk {
	int height[chunk_size][chunk_size];
};

class WorldGenerator {
public:
	int get_height(const glm::ivec2 &position);

private:
	const WorldGeneratorChunk &generate_chunk(const glm::ivec2 &position);
	int generator_get_height(const glm::ivec2 &global_position);

	std::map<ivec2_key, WorldGeneratorChunk> chunks;
};

int WorldGenerator::get_height(const glm::ivec2 &position) {
	const glm::ivec2 chunk_pos = get_chunk_pos_based_on_block_inside(position);
	const glm::ivec2 block_pos_in_chunk = position - chunk_pos * chunk_size;
	const auto it = this->chunks.find(chunk_pos);
	
	if(it == this->chunks.end()) {
		const auto &chunk = this->generate_chunk(chunk_pos);
		return chunk.height[block_pos_in_chunk.x][block_pos_in_chunk.y];
	}
	
	return it->second.height[block_pos_in_chunk.x][block_pos_in_chunk.y];
}

const WorldGeneratorChunk &WorldGenerator::generate_chunk(const glm::ivec2 &chunk_position) {
	auto &chunk = this->chunks[chunk_position];

	for(int i = 0; i < chunk_size; i++) {
		for(int j = 0; j < chunk_size; j++) {
			const glm::ivec2 chunk_pos = {i, j};
			const glm::ivec2 global_pos = chunk_position * chunk_size + chunk_pos;
			chunk.height[i][j] = this->generator_get_height(global_pos);
		}
	}

	return chunk;
}

int WorldGenerator::generator_get_height(const glm::ivec2 &global_position) {
	static auto random_engine = std::default_random_engine();
	static auto distribution = std::uniform_int_distribution(-3 * chunk_size, 3 * chunk_size);
	return distribution(random_engine);
}


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
	BlockId operator[](const glm::ivec3 &world_pos) const;

	void gen_chunks();
	void gen_chunks(const int quantity);
	void draw() const;

private:
	std::map<ivec3_key, Chunk> chunks;
	std::queue<glm::ivec3> chunks_to_generate;
	WorldGenerator generator;
	int radius;
};


class Chunk {
public:
	Chunk() = default;

	Chunk(const glm::ivec3 &position, WorldGenerator &generator) :
		built_buffer(false),
		drawable(false),
		position(position)
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
					this->blocks[i][j][k] = BlockId::STONE;
				
				for(; j < block_chunk_height && j < chunk_size; j++)
					this->blocks[i][j][k] = BlockId::DIRT;
				
				if(block_chunk_height >= 0 && block_chunk_height < chunk_size)
					this->blocks[i][j++][k] = BlockId::GRASS;
				
				for(; j < chunk_size; j++)
					this->blocks[i][j][k] = BlockId::AIR;
			}
		}
	}

	BlockId operator[](const glm::ivec3 &position) const {
		if(
			position.x < 0 || position.x >= chunk_size ||
			position.y < 0 || position.y >= chunk_size ||
			position.z < 0 || position.z >= chunk_size
		)
			return BlockId::NONE;
		return this->blocks[position.x][position.y][position.z];
	}

	void build_buffer(const Chunks &chunks) {
		if(this->built_buffer)
			return;
		
		this->built_buffer = true;

		
		std::vector<unsigned> indices;
		std::vector<Vertex> vertices;

		for(unsigned i = 0; i < chunk_size; i++) {
			for(unsigned j = 0; j < chunk_size; j++) {
				for(unsigned k = 0; k < chunk_size; k++) {
					const glm::ivec3 block_chunk_pos = {i, j, k};
					const glm::ivec3 block_world_pos = this->position * chunk_size + block_chunk_pos;
					
					const BlockId block_id = (*this)[block_chunk_pos];
					const BlockData &block = get_block(block_id);
					if(block.invisible)
						continue;

					static const std::array<glm::ivec3, (unsigned char) FaceId::NUM_FACES> relative_pos = {{
						{ 0,  0, -1},
						{ 0,  0,  1},
						{ 1,  0,  0},
						{-1,  0,  0},
						{ 0,  1,  0},
						{ 0, -1,  0}
					}};

					for(unsigned char face_id = 0; face_id < (unsigned char) FaceId::NUM_FACES; face_id++) {
						const glm::ivec3 near_block_chunk_pos = block_chunk_pos + relative_pos[(unsigned char) face_id];
						BlockId near_block_id = (*this)[near_block_chunk_pos];
						if(near_block_id == BlockId::NONE) {
							const glm::ivec3 near_block_global_pos = this->position * chunk_size + near_block_chunk_pos;
							near_block_id = chunks[near_block_global_pos];

							if(near_block_id == BlockId::NONE)
								continue;
						}
						
						const BlockData &near_block = get_block(near_block_id);
						if(near_block.invisible)
							build_block_face(block_world_pos, block_id, (FaceId) face_id, vertices, indices);
					}
				}
			}
		}


		if(indices.size() == 0)
			return;
		

		static const std::array<LayoutElement, 5> layout = {{
			{3, GL_FLOAT, false},
			{3, GL_FLOAT, false},
			{2, GL_FLOAT, false},
			{2, GL_FLOAT, false},
			{1, GL_FLOAT, false}
		}};

		new(&this->buffer) SuperBuffer<unsigned>(
			indices, gl::Usage::STATIC_DRAW,
			vertices, gl::Usage::STATIC_DRAW,
			layout, 0);
		this->drawable = true;
	}

	void draw() const {
		if(this->drawable)
			this->buffer.draw();
	}
	

private:
	bool built_buffer: 1;
	bool drawable: 1;
	BlockId blocks[chunk_size][chunk_size][chunk_size];
	glm::ivec3 position;
	SuperBuffer<unsigned> buffer;
};




Chunks::Chunks(const int radius) :
	radius(radius)
{
	if(this->radius > 0)
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

void Chunks::gen_chunks() {
	while(!this->chunks_to_generate.empty()) {
		const auto &pos = this->chunks_to_generate.front();
		Chunk &chunk = this->chunks[pos];		
		new (&chunk) Chunk(pos, this->generator);
		this->chunks_to_generate.pop();
	}

	for(auto &chunk : this->chunks)
		chunk.second.build_buffer(*this); // Gambiarra tbm
}

void Chunks::gen_chunks(const int quantity) {
	for(int i = 0; i < quantity; i++) {
		if(this->chunks_to_generate.empty())
			return;
		
		const auto &pos = this->chunks_to_generate.front();
		Chunk &chunk = this->chunks[pos];		
		new (&chunk) Chunk(pos, this->generator);
		this->chunks_to_generate.pop();

		chunk.build_buffer(*this); // Gambiarra por enquanto
	}
}


BlockId Chunks::operator[](const glm::ivec3 &block_world_pos) const {
	const glm::ivec3 chunk_pos = get_chunk_pos_based_on_block_inside(block_world_pos);
	
	const auto it = this->chunks.find(chunk_pos);
	if(it == this->chunks.end())
		return BlockId::NONE;
	
	const glm::ivec3 chunk_block_pos = block_world_pos - chunk_pos * chunk_size;
	return it->second[chunk_block_pos];
}

void Chunks::draw() const {
	for(const auto &chunk : this->chunks)
		chunk.second.draw();
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
	player.camera.speed = 10;
	player.camera.sensitivity = 3;
	
	
	auto chunks = Chunks(3);

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
		chunks.gen_chunks(1);
		
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
