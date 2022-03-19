
#include "chunks.hpp"

#include "utils.hpp"


Chunks::Chunks(const int radius, const glm::vec3 &camera_position) :
	radius(radius),
	last_chunk_position(get_chunk_pos_based_on_block_inside(det::to_int(camera_position)))
{
	const auto shaders = {
		Shader("resources/shaders/main.vert"),
		Shader("resources/shaders/main.frag")
	};

	new (&this->program) Program(shaders);
	
	const auto u_texture = this->program.get_uniform("u_texture");
	u_texture.set(0);

	this->u_mvp = this->program.get_uniform("u_mvp");
	this->u_offset = this->program.get_uniform("u_offset");

	this->generate_chunk_generation_queue();
}

Block::Id Chunks::get_block(const glm::ivec3 &block_global_position) const {
	const glm::ivec3 chunk_pos = get_chunk_pos_based_on_block_inside(block_global_position);
	
	const auto it = this->chunks.find(chunk_pos);
	if(it == this->chunks.end())
		return Block::Id::NONE;
	
	const glm::ivec3 chunk_block_pos = block_global_position - chunk_pos * chunk_size;
	return it->second.get_block_id(chunk_block_pos);
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
	while(!this->chunks_to_generate.empty())
		this->gen_one_chunk();
}

void Chunks::gen_chunks(const int quantity) {
	for(int i = 0; i < quantity; i++) {
		if(this->chunks_to_generate.empty())
			return;
		
		this->gen_one_chunk();
	}
}

static int infinite_norm(const glm::ivec3 &v) {
	return std::max(std::abs(v[0]), std::max(std::abs(v[1]), std::abs(v[2])));
}

void Chunks::draw(const glm::mat4 &mvp, const Renderer &renderer, const glm::vec3 &player_pos) const {
	const glm::ivec3 center_chunk_pos = get_chunk_pos_based_on_block_inside(det::to_int(player_pos));
	
	renderer.disable(gl::Capability::BLEND);

	this->program.bind();
	this->u_mvp.set(mvp);

	for(const auto &[position, chunk] : this->chunks) {
		const glm::ivec3 relative_distance = position - center_chunk_pos;
		if(infinite_norm(relative_distance) <= this->radius) {
			this->u_offset.set(glm::vec3(relative_distance * chunk_size));
			chunk.draw(renderer);
		}
	}
}

void Chunks::update(const glm::vec3 &camera_position) {
	const glm::ivec3 int_pos = det::to_int(camera_position);
	const glm::ivec3 chunk_position = get_chunk_pos_based_on_block_inside(int_pos);

	if(chunk_position != this->last_chunk_position) {
		this->last_chunk_position = chunk_position;
		this->generate_chunk_generation_queue();
	}

	for(auto &chunk : this->chunks)
		chunk.second.build_buffer_if_necessary(*this);
}

void Chunks::modify_block(const glm::ivec3 &block_global_pos, const Block::Id block_id) {
	const glm::ivec3 chunk_position = get_chunk_pos_based_on_block_inside(block_global_pos);
	const glm::ivec3 block_position_inside_chunk = block_global_pos - chunk_position * chunk_size;

	const auto it = this->chunks.find(chunk_position);
	if(it != this->chunks.end()) {
		it->second.set_block(block_position_inside_chunk, block_id);

		if(block_position_inside_chunk.x == 0) {
			const glm::ivec3 side_chunk_pos = chunk_position + BlockFace::get_block_relative_position_of_face(FaceId::WEST);
			this->mark_chunk_for_update_if_chunk_exist(side_chunk_pos);
		} else if(block_position_inside_chunk.x == chunk_size - 1) {
			const glm::ivec3 side_chunk_pos = chunk_position + BlockFace::get_block_relative_position_of_face(FaceId::EAST);
			this->mark_chunk_for_update_if_chunk_exist(side_chunk_pos);
		}

		if(block_position_inside_chunk.y == 0) {
			const glm::ivec3 side_chunk_pos = chunk_position + BlockFace::get_block_relative_position_of_face(FaceId::BOTTOM);
			this->mark_chunk_for_update_if_chunk_exist(side_chunk_pos);
		} else if(block_position_inside_chunk.y == chunk_size - 1) {
			const glm::ivec3 side_chunk_pos = chunk_position + BlockFace::get_block_relative_position_of_face(FaceId::TOP);
			this->mark_chunk_for_update_if_chunk_exist(side_chunk_pos);
		}

		if(block_position_inside_chunk.z == 0) {
			const glm::ivec3 side_chunk_pos = chunk_position + BlockFace::get_block_relative_position_of_face(FaceId::SOUTH);
			this->mark_chunk_for_update_if_chunk_exist(side_chunk_pos);
		} else if(block_position_inside_chunk.z == chunk_size - 1) {
			const glm::ivec3 side_chunk_pos = chunk_position + BlockFace::get_block_relative_position_of_face(FaceId::NORTH);
			this->mark_chunk_for_update_if_chunk_exist(side_chunk_pos);
		}
	}
}

void Chunks::gen_one_chunk() {
	assert(!this->chunks_to_generate.empty());

	const auto pos = this->chunks_to_generate.front();
	this->chunks_to_generate.pop();

	Chunk &chunk = this->chunks[pos];
	new (&chunk) Chunk(pos, this->generator);

	for(unsigned char i = 0; i < (unsigned char) FaceId::NUM_FACES; i++) {
		const glm::ivec3 side_chunk_pos = pos + BlockFace::get_block_relative_position_of_face((FaceId) i);
		const auto it = this->chunks.find(side_chunk_pos);
		if(it != this->chunks.end())
			it->second.mark_for_update();	
	}
}

Chunks::ivec3_key::ivec3_key(const glm::ivec3 &vec) :
	glm::ivec3(vec) {}

bool Chunks::ivec3_key::operator<(const ivec3_key &other) const {
	for(unsigned char i = 0; i < 3; i++)
		if((*this)[i] != other[i])
			return (*this)[i] < other[i];
	return false;
}

void Chunks::generate_chunk_generation_queue() {
	this->chunks_to_generate = std::queue<glm::ivec3>();


	for(int distance = 0; distance <= this->radius; distance++) {
		for(int i = -distance; i <= distance; i++) {
			for(int j = -distance; j <= distance; j++) {
				const glm::ivec3 pos = {i, distance, j};
				this->add_chunk_position_to_queue_if_dont_exist(this->last_chunk_position + pos);
				this->add_chunk_position_to_queue_if_dont_exist(this->last_chunk_position - pos);
			}
		}

		for(int i = -distance; i <= distance; i++) {
			for(int j = -distance + 1; j < distance; j++) {
				const glm::ivec3 pos = {i, j, distance};
				this->add_chunk_position_to_queue_if_dont_exist(this->last_chunk_position + pos);
				this->add_chunk_position_to_queue_if_dont_exist(this->last_chunk_position - pos);
			}
		}

		for(int i = -distance + 1; i < distance; i++) {
			for(int j = -distance + 1; j < distance; j++) {
				const glm::ivec3 pos = {distance, i, j};
				this->add_chunk_position_to_queue_if_dont_exist(this->last_chunk_position + pos);
				this->add_chunk_position_to_queue_if_dont_exist(this->last_chunk_position - pos);
			}
		}
	}
}

void Chunks::add_chunk_position_to_queue_if_dont_exist(const glm::ivec3 &chunk_pos) {
	const auto it = this->chunks.find(chunk_pos);
	if(it == this->chunks.end())
		this->chunks_to_generate.push(chunk_pos);
}

void Chunks::mark_chunk_for_update_if_chunk_exist(const glm::ivec3 &chunk_pos) {
	const auto it = this->chunks.find(chunk_pos);
	if(it != this->chunks.end())
		it->second.mark_for_update();
}
