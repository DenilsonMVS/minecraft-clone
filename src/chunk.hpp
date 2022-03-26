
#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <glm/glm.hpp>
#include <array>

#include "index_buffer.hpp"
#include "framebuffer.hpp"

#include "world_generator.hpp"
#include "blocks/drawable_block.hpp"
#include "super_buffer.hpp"
#include "renderer.hpp"

#include "chunk_definitions.hpp"


class Chunks;

class Chunk {
public:
	Chunk() = default;
	Chunk(const glm::ivec3 &position, WorldGenerator &generator);

	Block::Id get_block_id(const glm::ivec3 &block_position_in_chunk) const;
	void set_block(const glm::ivec3 &block_position_in_chunk, const Block::Id block_id);
	bool build_buffer_if_necessary(const Chunks &chunks);
	//bool build_only_non_transparent_buffer(const Chunks &chunks);
	//bool transparent_buffer_sort(const Chunks &chunks, const glm::ivec3 &central_block);

	void mark_for_update();
	void draw_non_transparent(const Renderer &renderer) const;
	void draw_transparent(const Renderer &renderer) const;

	bool can_draw_non_transparent_mesh() const;
	bool can_draw_transparent_mesh() const;

	static glm::vec3 offset_to_draw(const glm::ivec3 &block_world_pos, const glm::vec3 &player_pos);

	static constexpr unsigned max_faces_in_chunk() {
		const unsigned num_blocks = chunk_size * chunk_size * chunk_size / 2; // Num of blocks that maximizes the number of faces
		return num_blocks * (unsigned) FaceId::NUM_FACES;
	}

private:
	bool need_update: 1;
	unsigned num_faces;
	unsigned transparent_faces;
	SuperBuffer main_buffer;
	SuperBuffer transparent_buffer;

public:
	glm::ivec3 position;

private:
	std::array<std::array<std::array<Block::Id, chunk_size>, chunk_size>, chunk_size> blocks;

	static const std::array<LayoutElement, 5> layout;
};

#endif
