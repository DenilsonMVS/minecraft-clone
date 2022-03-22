
#include "block_face.hpp"

#include <vector>

#include "block_texture_atlas.hpp"


const BlockFace &BlockFace::get_block_face(const BlockFace::Id id) {
	assert((unsigned char) id < (unsigned char) BlockFace::Id::NUM_FACES);
	return block_faces[(size_t) id];
}

const glm::ivec3 &BlockFace::get_block_relative_position_of_face(const FaceId id) {
	assert((unsigned char) id < (unsigned char) FaceId::NUM_FACES);
	return relative_pos[(unsigned char) id];
}

const std::array<LayoutElement, 5> BlockFace::block_face_vertex_layout = {{
	{3, gl::get_enum<float>(), false},
	{3, gl::get_enum<float>(), false},
	{2, gl::get_enum<float>(), false},
	{2, gl::get_enum<float>(), false},
	{1, gl::get_enum<float>(), false}
}};

const std::array<LayoutElement, 3> BlockFace::transparent_block_face_vertex_layout = {{
	{3, gl::get_enum<float>(), false},
	{2, gl::get_enum<float>(), false},
	{1, gl::get_enum<float>(), false}
}};

const std::array<std::array<glm::ivec3, num_vertices_per_face>, (unsigned char) FaceId::NUM_FACES> BlockFace::face_positions_helper_data = {{
	{{{0, 0, 1}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1}}},
	{{{1, 0, 0}, {1, 1, 0}, {0, 0, 0}, {0, 1, 0}}},
	{{{1, 0, 1}, {1, 1, 1}, {1, 0, 0}, {1, 1, 0}}},
	{{{0, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 1, 1}}},
	{{{0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}}},
	{{{0, 0, 0}, {0, 0, 1}, {1, 0, 0}, {1, 0, 1}}}
}};

const std::array<BlockFace, (size_t) BlockFace::Id::NUM_FACES> BlockFace::block_faces = {{
	{"air.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::AIR)},
	{"dirt.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::DIRT)},
	{"grass_block_side_overlay.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::GRASS_SIDE)},
	{"grass_block_top.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::GRASS_TOP)},
	{"stone.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::STONE)},
	{"selection.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::SELECTION)},
	{"water.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::WATER)},
	{"glass.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::GLASS)}
}};

const std::array<glm::ivec3, (unsigned char) FaceId::NUM_FACES> BlockFace::relative_pos = {{
	{ 0,  0,  1},
	{ 0,  0, -1},
	{ 1,  0,  0},
	{-1,  0,  0},
	{ 0,  1,  0},
	{ 0, -1,  0}
}};
