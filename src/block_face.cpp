
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

IndexBuffer<unsigned> BlockFace::gen_index_buffer_to_gen_faces(const unsigned num_faces) {
	auto indices = std::vector<unsigned>(num_faces * num_indices_per_face);
	
	static const std::array<unsigned, num_indices_per_face> indices_base = {{
		2, 1, 0,
		2, 3, 1
	}};

	for(unsigned i = 0; i < num_faces; i++)
		for(unsigned j = 0; j < num_indices_per_face; j++)
			indices[i * num_indices_per_face + j] = i * num_vertices_per_face + indices_base[j];
	
	return IndexBuffer<unsigned>(indices);
}

const std::array<LayoutElement, 5> BlockFace::block_face_vertex_layout = {{
	{3, gl::get_enum<float>(), false},
	{3, gl::get_enum<float>(), false},
	{2, gl::get_enum<float>(), false},
	{2, gl::get_enum<float>(), false},
	{1, gl::get_enum<float>(), false}
}};

const std::array<BlockFace, (size_t) BlockFace::Id::NUM_FACES> BlockFace::block_faces = {{
	{"air.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::AIR)},
	{"dirt.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::DIRT)},
	{"grass_block_side_overlay.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::GRASS_SIDE)},
	{"grass_block_top.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::GRASS_TOP)},
	{"stone.png", BlockTextureAtlas::get_texture_coords_of_face(BlockFace::Id::STONE)}
}};

const std::array<glm::ivec3, (unsigned char) FaceId::NUM_FACES> BlockFace::relative_pos = {{
	{ 0,  0, -1},
	{ 0,  0,  1},
	{ 1,  0,  0},
	{-1,  0,  0},
	{ 0,  1,  0},
	{ 0, -1,  0}
}};
