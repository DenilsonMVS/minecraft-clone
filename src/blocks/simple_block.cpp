
#include "simple_block.hpp"


SimpleBlock::SimpleBlock(const BlockFace::Id face) :
	DrawableBlock(false, true),
	face(BlockFace::get_block_face(face)) {}

void SimpleBlock::append_face_vertices(
	const glm::ivec3 &block_global_position,
	const FaceId face,
	std::vector<BlockFaceVertex> &vertices,
	std::vector<TransparentBlockFaceVertex> &transparent_vertices) const
{
	this->build_non_transparent(
		block_global_position,
		face,
		this->face.coords,
		{0, 0},
		vertices);
}
