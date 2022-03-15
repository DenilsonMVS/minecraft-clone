
#include "block_face.hpp"

#include "block_texture_atlas.hpp"

const BlockFace BlockFace::block_faces[(unsigned char) BlockFace::Id::NUM_FACES] = {
	{"air.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::AIR)},
	{"dirt.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::DIRT)},
	{"grass_block_side_overlay.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::GRASS_SIDE)},
	{"grass_block_top.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::GRASS_TOP)},
	{"stone.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::STONE)}
};
