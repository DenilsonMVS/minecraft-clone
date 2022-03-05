
#ifndef BLOCK_FACES_HPP
#define BLOCK_FACES_HPP

#include "block_texture_atlas.hpp"

constexpr FaceData block_faces[] = {
	{"", BlockTextureAtlas::get_texture_coords_of_face(BlockFaceId::AIR)},
	{"dirt.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFaceId::DIRT)},
	{"grass_block_side_overlay.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFaceId::GRASS_SIDE)},
	{"grass_block_top.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFaceId::GRASS_TOP)}
};

#endif
