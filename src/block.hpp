
#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "block_face.hpp"


struct Block {
	enum class Id : unsigned char {
		AIR,
		DIRT,
		GRASS,
		STONE,
		NUM_BLOCKS,
		NONE
	};


	static const Block &get_block(const Id id);
	static const BlockFace &get_block_face(const Id id, const FaceId face, const FaceLayer layer);
	static const TextureCoords &get_coords(const Id id, const FaceId face, const FaceLayer layer);

	bool invisible: 1;
	BlockFace::Id faces[(unsigned char) FaceId::NUM_FACES][(unsigned char) FaceLayer::NUM_LAYERS];

	static const Block blocks[(unsigned char) Id::NUM_BLOCKS];
};


#endif
