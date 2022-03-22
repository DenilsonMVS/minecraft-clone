
#include "biome_block.hpp"


BiomeBlock::BiomeBlock(const BlockFace::Id main_face, const BlockFace::Id secondary_face) :
	DrawableBlock(false, true),
	main_face(&BlockFace::get_block_face(main_face)),
	secondary_face(&BlockFace::get_block_face(secondary_face)) {}


void BiomeBlock::append_face_vertices(
	const glm::ivec3 &block_global_position,
	const FaceId face,
	std::vector<BlockFaceVertex> &vertices,
	std::vector<TransparentBlockFaceVertex> &transparent_vertices) const
{
	this->build_non_transparent(block_global_position, face, this->main_face->coords, this->secondary_face->coords, vertices);
}
