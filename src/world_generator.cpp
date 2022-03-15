
#include "world_generator.hpp"


WorldGenerator::ivec2_key::ivec2_key(const glm::ivec2 &vec) :
	glm::ivec2(vec) {}

bool WorldGenerator::ivec2_key::operator<(const ivec2_key &other) const {
	for(unsigned char i = 0; i < 2; i++)
		if((*this)[i] != other[i])
			return (*this)[i] < other[i];
	return false;
}


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
	const float max_perlin_amplitude = std::sqrt(0.5);

	const float f1 = 1.0f / 256, a1 = 64 / max_perlin_amplitude;
	const float f2 = 1.0f / 128, a2 = 32 / max_perlin_amplitude;
	const float f3 = 1.0f /  64, a3 = 16 / max_perlin_amplitude;
	const float f4 = 1.0f /  32, a4 = 8  / max_perlin_amplitude;
	const float f5 = 1.0f /  16, a5 = 4  / max_perlin_amplitude;
	const float f6 = 1.0f /   8, a6 = 2  / max_perlin_amplitude;
	const float f7 = 1.0f /   4, a7 = 1  / max_perlin_amplitude;

	const auto pos = glm::vec3(block_global_pos.x, 0.0f, block_global_pos.y);


	return
		a1 * noise_generator.noise(pos * f1) +
		a2 * noise_generator.noise(pos * f2) +
		a3 * noise_generator.noise(pos * f3) +
		a4 * noise_generator.noise(pos * f4) +
		a5 * noise_generator.noise(pos * f5) +
		a6 * noise_generator.noise(pos * f6) +
		a7 * noise_generator.noise(pos * f7);
}
