
#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <vector>
#include <array>

#include "block_face.hpp"


class Block {
public:
	enum class Id : unsigned char {
		AIR,
		DIRT,
		GRASS,
		STONE,
		WATER,
		NUM_BLOCKS,
		NONE
	};


	void append_face_vertices(
		const glm::ivec3 &block_global_position,
		const FaceId face,
		std::vector<BlockFaceVertex> &vertices,
		std::vector<TransparentBlockFaceVertex> &transparent_vertices) const;
	
	const BlockFace &get_block_face(const FaceId face, const FaceLayer layer) const;

	bool invisible: 1;
	bool transparent: 1;
	bool solid: 1;
	BlockFace::Id faces[(unsigned char) FaceId::NUM_FACES][(unsigned char) FaceLayer::NUM_LAYERS];

	static const Block &get_block(const Id id);

private:
	static const std::array<Block, (size_t) Id::NUM_BLOCKS> blocks;
};


#endif
