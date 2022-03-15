
#ifndef BLOCK_FACE_HPP
#define BLOCK_FACE_HPP

#include "texture_coords.hpp"


enum class FaceId : unsigned char {
	NORTH,
	SOUTH,
	EAST,
	WEST,
	TOP,
	BOTTOM,
	NUM_FACES
};

enum class FaceLayer : unsigned char {
	MAIN,
	SECONDARY,
	NUM_LAYERS,
	AIR
};

struct BlockFace {
	const char *source;
	TextureCoords coords;

	enum class Id : unsigned char {
		AIR,
		DIRT,
		GRASS_SIDE,
		GRASS_TOP,
		STONE,
		NUM_FACES,
		NONE
	};

	static const BlockFace block_faces[(unsigned char) Id::NUM_FACES];
};

#endif
