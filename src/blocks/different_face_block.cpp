
#include "different_face_block.hpp"


DifferentFaceBlock::DifferentFaceBlock(
	const BlockFace::Id north,
	const BlockFace::Id south,
	const BlockFace::Id east,
	const BlockFace::Id west,
	const BlockFace::Id top,
	const BlockFace::Id bottom) :
	DrawableBlock(false, true),
	north(&BlockFace::get_block_face(north)),
	south(&BlockFace::get_block_face(south)),
	east(&BlockFace::get_block_face(east)),
	west(&BlockFace::get_block_face(west)),
	top(&BlockFace::get_block_face(top)),
	bottom(&BlockFace::get_block_face(bottom)) {}


void DifferentFaceBlock::append_face_vertices(
	const glm::ivec3 &block_position,
	const FaceId face,
	std::vector<BlockFaceVertex> &vertices,
	std::vector<TransparentBlockFaceVertex> &transparent_vertices) const
{
	this->build_non_transparent(
		block_position,
		face,
		this->faces[(unsigned char) face]->coords,
		{0, 0},
		vertices);
}
