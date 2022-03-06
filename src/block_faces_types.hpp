
#ifndef BLOCK_FACES_TYPES_HPP
#define BLOCK_FACES_TYPES_HPP


struct TextureCoords {
	float x_min, y_min;
	float x_max, y_max;
};


enum class FaceId : unsigned char {
	NORTH,
	SOUTH,
	EAST,
	WEST,
	TOP,
	BOTTOM,
	NUM_FACES
};

enum class BlockFaceId : unsigned char {
	DIRT,
	GRASS_SIDE,
	GRASS_TOP,
	NUM_FACES,
	NO_FACE
};

struct FaceData {
	const char * const source;
	TextureCoords coords;
};

#endif
