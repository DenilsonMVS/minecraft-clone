
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

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



constexpr int chunk_size = 32;


struct MapKey : public glm::ivec3 {
	MapKey(const glm::ivec3 &vec);
	bool operator<(const MapKey &other) const;
};

MapKey::MapKey(const glm::ivec3 &vec) :
	glm::ivec3(vec) {}

bool MapKey::operator<(const MapKey &other) const {
	if(this->x < other.x)
		return true;
	
	if(this->x == other.x && this->y < other.y)
		return true;
	
	if(this->y == other.y && this->z < other.z)
		return true;
	
	return false;
}

class Chunk;

class Chunks {
public:
	Chunks(const glm::ivec3 &quantity);
	BlockId operator[](const glm::ivec3 &world_pos) const;

	void draw() const;

private:
	std::map<MapKey, Chunk> chunks;
};


class Chunk {
public:
	Chunk() = default;

	Chunk(const glm::ivec3 &position) :
		built_buffer(false),
		drawable(false),
		position(position)
	{
		for(unsigned i = 0; i < chunk_size; i++)
			for(unsigned j = 0; j < chunk_size; j++)
				for(unsigned k = 0; k < chunk_size; k++)
					this->blocks[i][j][k] = BlockId::AIR;
		
		for(unsigned i = 1; i < chunk_size - 1; i++)
			for(unsigned j = 1; j < chunk_size - 1; j++)
				for(unsigned k = 1; k < chunk_size - 1; k++)
					this->blocks[i][j][k] = BlockId::GRASS;

		for(unsigned i = 1; i < chunk_size - 1; i++)
			this->blocks[i][5][5] = BlockId::AIR;
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
					
					static const glm::ivec3 relative_pos[(unsigned char) FaceId::NUM_FACES] = {
						{ 0,  0, -1},
						{ 0,  0,  1},
						{ 1,  0,  0},
						{-1,  0,  0},
						{ 0,  1,  0},
						{ 0, -1,  0}
					};


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
		

		static const LayoutElement layout[] = {
			{3, GL_FLOAT, false},
			{3, GL_FLOAT, false},
			{2, GL_FLOAT, false},
			{2, GL_FLOAT, false},
			{1, GL_FLOAT, false}
		};

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




Chunks::Chunks(const glm::ivec3 &quantity) {
	for(int i = 0; i < quantity.x; i++) {
		for(int j = 0; j < quantity.y; j++) {
			for(int k = 0; k < quantity.z; k++) {
				const glm::ivec3 pos = {i, j, k};
				this->chunks[pos] = Chunk(pos);
			}
		}
	}

	for(auto &chunk : this->chunks)
		chunk.second.build_buffer(*this);
}

BlockId Chunks::operator[](const glm::ivec3 &world_pos) const {
	const glm::ivec3 chunk_pos = world_pos / chunk_size;
	const glm::ivec3 block_pos = world_pos - chunk_pos * chunk_size;
	
	const auto it = this->chunks.find(chunk_pos);
	if(it == this->chunks.end())
		return BlockId::NONE;

	return it->second[block_pos];
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
	player.camera.speed = 3;
	player.camera.sensitivity = 3;
	

	auto chunks = Chunks({4, 4, 4});
	
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


		chunks.draw();

		window.swap_buffers();
		Renderer::poll_events();
	}

	return 0;
}
