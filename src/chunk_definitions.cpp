
#include "chunk_definitions.hpp"


glm::ivec2 get_chunk_pos_based_on_block_inside(const glm::ivec2 &block_global_pos) {
	return {
		block_global_pos.x < 0 ? (block_global_pos.x + 1) / chunk_size - 1 : block_global_pos.x / chunk_size,
		block_global_pos.y < 0 ? (block_global_pos.y + 1) / chunk_size - 1 : block_global_pos.y / chunk_size
	};
}

glm::ivec3 get_chunk_pos_based_on_block_inside(const glm::ivec3 &block_global_pos) {
	return {
		block_global_pos.x < 0 ? (block_global_pos.x + 1) / chunk_size - 1 : block_global_pos.x / chunk_size,
		block_global_pos.y < 0 ? (block_global_pos.y + 1) / chunk_size - 1 : block_global_pos.y / chunk_size,
		block_global_pos.z < 0 ? (block_global_pos.z + 1) / chunk_size - 1 : block_global_pos.z / chunk_size
	};
}
