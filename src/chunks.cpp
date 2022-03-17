
#include "chunks.hpp"

#include "utils.hpp"


Chunks::Chunks(const int radius, const glm::vec3 &camera_position) :
	radius(radius),
	last_chunk_position(get_chunk_pos_based_on_block_inside(det::to_int(camera_position)))
{
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

void Chunks::draw(const IndexBuffer<unsigned> &ibo) const {
	for(const auto &[position, chunk] : this->chunks) {
		const glm::ivec3 relative_distance = position - this->last_chunk_position;
		if(infinite_norm(relative_distance) <= this->radius)
			chunk.draw(ibo);
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
