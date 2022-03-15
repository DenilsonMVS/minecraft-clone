
#include "block_face.hpp"

#include "block_texture_atlas.hpp"

const std::array<BlockFace, (size_t) BlockFace::Id::NUM_FACES> BlockFace::block_faces = {{
	{"air.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::AIR)},
	{"dirt.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::DIRT)},
	{"grass_block_side_overlay.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::GRASS_SIDE)},
	{"grass_block_top.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::GRASS_TOP)},
	{"stone.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::STONE)}
}};

const BlockFace &BlockFace::get_block_face(const BlockFace::Id id) {
	assert((unsigned char) id < (unsigned char) BlockFace::Id::NUM_FACES);
	return block_faces[(size_t) id];
}
