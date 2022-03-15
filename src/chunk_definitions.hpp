
#ifndef CHUNK_DEFINITIONS_HPP
#define CHUNK_DEFINITIONS_HPP

#include <glm/glm.hpp>


static constexpr int chunk_size = 32;

glm::ivec2 get_chunk_pos_based_on_block_inside(const glm::ivec2 &block_global_pos);
glm::ivec3 get_chunk_pos_based_on_block_inside(const glm::ivec3 &block_global_pos);

#endif
