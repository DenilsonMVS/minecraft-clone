
#include "block.hpp"

const BlockData &get_block(const BlockId id) {
	return blocks[(unsigned char) id];
}

const FaceData &get_block_face(const BlockId id, const FaceId face, const FaceLayer layer) {
	return block_faces[(unsigned char) get_block(id).faces[(unsigned char) face][(unsigned char) layer]];
}
const TextureCoords &get_coords(const BlockId id, const FaceId face, const FaceLayer layer) {
	return get_block_face(id, face, layer).coords;
}
