
#include "chunks.hpp"


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

void Chunks::draw(const IndexBuffer<unsigned> &ibo) const {
	for(const auto &chunk : this->chunks)
		chunk.second.draw(ibo);
}

void Chunks::update() {
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