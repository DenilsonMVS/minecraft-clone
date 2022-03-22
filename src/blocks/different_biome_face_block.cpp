
#include "different_biome_face_block.hpp"


DifferentBiomeFaceBlock::DifferentBiomeFaceBlock(
	const BlockFace::Id main_north,
	const BlockFace::Id main_south,
	const BlockFace::Id main_east,
	const BlockFace::Id main_west,
	const BlockFace::Id main_top,
	const BlockFace::Id main_bottom,
	const BlockFace::Id secondary_north,
	const BlockFace::Id secondary_south,
	const BlockFace::Id secondary_east,
	const BlockFace::Id secondary_west,
	const BlockFace::Id secondary_top,
	const BlockFace::Id secondary_bottom) :
	DifferentFaceBlock(main_north, main_south, main_east, main_west, main_top, main_bottom),
	secondary_north(&BlockFace::get_block_face(secondary_north)),
	secondary_south(&BlockFace::get_block_face(secondary_south)),
	secondary_east(&BlockFace::get_block_face(secondary_east)),
	secondary_west(&BlockFace::get_block_face(secondary_west)),
	secondary_top(&BlockFace::get_block_face(secondary_top)),
	secondary_bottom(&BlockFace::get_block_face(secondary_bottom)) {}

void DifferentBiomeFaceBlock::append_face_vertices(
	const glm::ivec3 &block_position,
	const FaceId face,
	std::vector<BlockFaceVertex> &vertices,
	std::vector<TransparentBlockFaceVertex> &transparent_vertices) const
{
	this->build_non_transparent(
		block_position,
		face, this->faces[(unsigned char) face]->coords,
		this->secondary_faces[(unsigned char) face]->coords,
		vertices);
}