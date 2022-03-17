
#include "world_generator.hpp"
#include <iostream>

WorldGenerator::ivec2_key::ivec2_key(const glm::ivec2 &vec) :
	glm::ivec2(vec) {}

bool WorldGenerator::ivec2_key::operator<(const ivec2_key &other) const {
	for(unsigned char i = 0; i < 2; i++)
		if((*this)[i] != other[i])
			return (*this)[i] < other[i];
	return false;
}

WorldGenerator::WorldGenerator() :
	noise_generator(1.0f / 128, 64) {}

int WorldGenerator::get_height(const glm::ivec2 &block_global_pos) {
	const glm::ivec2 chunk_pos = get_chunk_pos_based_on_block_inside(block_global_pos);
	const glm::ivec2 block_pos_in_chunk = block_global_pos - chunk_pos * chunk_size;
	const auto it = this->chunks.find(chunk_pos);
	
	if(it == this->chunks.end()) {
		const auto &chunk = this->generate_chunk(chunk_pos);
		return chunk.height[block_pos_in_chunk.x][block_pos_in_chunk.y];
	}
	
	return it->second.height[block_pos_in_chunk.x][block_pos_in_chunk.y];
}

const WorldGenerator::WorldGeneratorChunk &WorldGenerator::generate_chunk(const glm::ivec2 &chunk_position) {
	auto &chunk = this->chunks[chunk_position];

	for(int i = 0; i < chunk_size; i++) {
		for(int j = 0; j < chunk_size; j++) {
			const glm::ivec2 chunk_pos = {i, j};
			const glm::ivec2 global_pos = chunk_position * chunk_size + chunk_pos;
			chunk.height[i][j] = this->noise_generator_get_height(global_pos);
		}
	}

	return chunk;
}

int WorldGenerator::noise_generator_get_height(const glm::ivec2 &block_global_pos) {
	const glm::vec2 float_pos = block_global_pos;
	return std::floor(this->noise_generator.fractal(5, float_pos));
}
