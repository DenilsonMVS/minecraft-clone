
#ifndef CHUNKS_HPP
#define CHUNKS_HPP

#include <glm/glm.hpp>
#include <map>
#include <queue>

#include "chunk.hpp"
#include "index_buffer.hpp"
#include "program.hpp"
#include "renderer.hpp"


class Chunks {
public:
	Chunks(const int radius, const glm::dvec3 &player_position);

	Block::Id get_block(const glm::ivec3 &block_global_position) const;

	Chunk &get_chunk(const glm::ivec3 &chunk_pos);
	const Chunk &get_chunk(const glm::ivec3 &chunk_pos) const;

	void gen_chunks();
	void gen_chunks(const int quantity);
	void draw(const glm::mat4 &mvp, const Renderer &renderer) const;
	void update(const glm::dvec3 &player_position);

	void modify_block(const glm::ivec3 &block_global_pos, const Block::Id block_id);

private:
	void gen_one_chunk();
	void generate_chunk_generation_queue();
	void add_chunk_position_to_queue_if_dont_exist(const glm::ivec3 &chunk_pos);
	void mark_chunk_for_update_if_chunk_exist(const glm::ivec3 &chunk_pos);
	void select_chunks_to_draw();

	bool transparent_chunk_mesh_is_sorted() const;


	struct ivec3_key : public glm::ivec3 {
		ivec3_key(const glm::ivec3 &vec);
		bool operator<(const ivec3_key &other) const;
	};

	std::map<ivec3_key, Chunk> chunks;
	WorldGenerator generator;
	int radius;
	glm::ivec3 center_chunk_position;

	std::queue<glm::ivec3> chunks_to_generate;
	std::vector<const Chunk *> chunks_to_draw_non_transparent;
	std::vector<const Chunk *> chunks_to_draw_transparent;

	Program program;
	Uniform u_mvp;
	Uniform u_offset;

	Program transparent_program;
	Uniform u_transparent_mvp;
	Uniform u_transparent_offset;
};

#endif
