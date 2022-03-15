
#ifndef CHUNK_DEFINITIONS_HPP
#define CHUNK_DEFINITIONS_HPP

#include <glm/glm.hpp>


static constexpr int chunk_size = 32;
static constexpr unsigned num_indices_per_face = 6;
static constexpr unsigned num_vertices_per_face = 4;

glm::ivec2 get_chunk_pos_based_on_block_inside(const glm::ivec2 &block_global_pos);
glm::ivec3 get_chunk_pos_based_on_block_inside(const glm::ivec3 &block_global_pos);

#endif
