
#ifndef CHUNKS_HPP
#define CHUNKS_HPP

#include <glm/glm.hpp>
#include <map>
#include <queue>

#include "block.hpp"
#include "chunk.hpp"
#include "index_buffer.hpp"


class Chunks {
public:
	Chunks(const int radius);

	Block::Id get_block(const glm::ivec3 &block_global_position) const;

	Chunk &get_chunk(const glm::ivec3 &chunk_pos);
	const Chunk &get_chunk(const glm::ivec3 &chunk_pos) const;

	void gen_chunks();
	void gen_chunks(const int quantity);
	void draw(const IndexBuffer<unsigned> &ibo) const;
	void update();

private:
	void gen_one_chunk();

	struct ivec3_key : public glm::ivec3 {
		ivec3_key(const glm::ivec3 &vec);
		bool operator<(const ivec3_key &other) const;
	};

	std::map<ivec3_key, Chunk> chunks;
	std::queue<glm::ivec3> chunks_to_generate;
	WorldGenerator generator;
	int radius;	
};

#endif
