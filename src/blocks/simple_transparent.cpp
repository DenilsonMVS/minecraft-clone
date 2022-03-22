
#include "simple_transparent.hpp"


SimpleTransparent::SimpleTransparent(const BlockFace::Id face) :
	TransparentDrawableBlock(true),
	face(BlockFace::get_block_face(face)) {}

void SimpleTransparent::append_face_vertices(
	const glm::ivec3 &block_position,
	const FaceId face,
	std::vector<BlockFaceVertex> &vertices,
	std::vector<TransparentBlockFaceVertex> &transparent_vertices) const
{
	this->build_transparent(block_position, face, this->face.coords, transparent_vertices);
}
