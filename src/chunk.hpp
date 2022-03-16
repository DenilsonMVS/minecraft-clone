
#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <glm/glm.hpp>
#include <array>
#include <vector>

#include "index_buffer.hpp"

#include "world_generator.hpp"
#include "block.hpp"
#include "super_buffer.hpp"

#include "chunk_definitions.hpp"


class Chunks;

class Chunk {
public:
	Chunk() = default;
	Chunk(const glm::ivec3 &position, WorldGenerator &generator);

	Block::Id get_block_id(const glm::ivec3 &block_position_in_chunk) const;
	void build_buffer_if_necessary(const Chunks &chunks);

	void mark_for_update();
	void draw(const IndexBuffer<unsigned> &ibo) const;

	static constexpr unsigned max_faces_in_chunk() {
		const unsigned num_blocks = chunk_size * chunk_size * chunk_size / 2; // Num of blocks that maximizes the number of faces
		return num_blocks * (unsigned) FaceId::NUM_FACES;
	}

private:
	bool drawable: 1;
	bool need_update: 1;
	unsigned num_faces;
	SuperBuffer buffer;

public:
	glm::ivec3 position;

private:
	std::vector<std::array<std::array<Block::Id, chunk_size>, chunk_size>> blocks;

	static const std::array<LayoutElement, 5> layout;
};

#endif
