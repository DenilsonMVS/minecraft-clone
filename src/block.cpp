
#include "block.hpp"

const BlockData &get_block(const BlockId id) {
	return blocks[(unsigned char) id];
}

const FaceData &get_block_face(const BlockId id, const FaceId face) {
	return block_faces[(unsigned char) get_block(id).faces[(unsigned char) face]];
}
const TextureCoords &get_coords(const BlockId id, const FaceId face) {
	return get_block_face(id, face).coords;
}
