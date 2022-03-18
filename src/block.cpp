
#include "block.hpp"

#include <cassert>
#include <iostream>


const std::array<Block, (size_t) Block::Id::NUM_BLOCKS> Block::blocks = {{
	{true, {
		{BlockFace::Id::AIR, BlockFace::Id::AIR},
		{BlockFace::Id::AIR, BlockFace::Id::AIR},
		{BlockFace::Id::AIR, BlockFace::Id::AIR},
		{BlockFace::Id::AIR, BlockFace::Id::AIR},
		{BlockFace::Id::AIR, BlockFace::Id::AIR},
		{BlockFace::Id::AIR, BlockFace::Id::AIR}}},
	{false, {
		{BlockFace::Id::DIRT, BlockFace::Id::AIR},
		{BlockFace::Id::DIRT, BlockFace::Id::AIR},
		{BlockFace::Id::DIRT, BlockFace::Id::AIR},
		{BlockFace::Id::DIRT, BlockFace::Id::AIR},
		{BlockFace::Id::DIRT, BlockFace::Id::AIR},
		{BlockFace::Id::DIRT, BlockFace::Id::AIR}}},
	{false, {
		{BlockFace::Id::DIRT, BlockFace::Id::GRASS_SIDE},
		{BlockFace::Id::DIRT, BlockFace::Id::GRASS_SIDE}, 
		{BlockFace::Id::DIRT, BlockFace::Id::GRASS_SIDE}, 
		{BlockFace::Id::DIRT, BlockFace::Id::GRASS_SIDE}, 
		{BlockFace::Id::AIR, BlockFace::Id::GRASS_TOP}, 
		{BlockFace::Id::DIRT, BlockFace::Id::AIR}}},
	{false, {
		{BlockFace::Id::STONE, BlockFace::Id::AIR},
		{BlockFace::Id::STONE, BlockFace::Id::AIR},
		{BlockFace::Id::STONE, BlockFace::Id::AIR},
		{BlockFace::Id::STONE, BlockFace::Id::AIR},
		{BlockFace::Id::STONE, BlockFace::Id::AIR},
		{BlockFace::Id::STONE, BlockFace::Id::AIR}}}
}};


void Block::append_face_vertices(const glm::ivec3 &block_global_position, const FaceId face, std::vector<BlockFaceVertex> &vertices) const {

	const auto &main_text_position = this->get_block_face(face, FaceLayer::MAIN).coords;
	const glm::vec2 vbo_main_text_positions[] = {
		{main_text_position.x_min, main_text_position.y_max},
		{main_text_position.x_min, main_text_position.y_min},
		{main_text_position.x_max, main_text_position.y_max},
		{main_text_position.x_max, main_text_position.y_min}
	};

	const auto &sec_text_position = this->get_block_face(face, FaceLayer::SECONDARY).coords;
	const glm::vec2 vbo_sec_text_positions[] = {
		{sec_text_position.x_min, sec_text_position.y_max},
		{sec_text_position.x_min, sec_text_position.y_min},
		{sec_text_position.x_max, sec_text_position.y_max},
		{sec_text_position.x_max, sec_text_position.y_min}
	};
	
	for(int i = 0; i < num_vertices_per_face; i++) {
		const BlockFaceVertex vertex = {
			block_global_position + BlockFace::face_positions_helper_data[(unsigned char) face][i],
			{0.1, 0.7, 0.15},
			vbo_main_text_positions[i],
			vbo_sec_text_positions[i],
			0.8};
		vertices.push_back(vertex);
	}
}

const BlockFace &Block::get_block_face(const FaceId face, const FaceLayer layer) const {
	assert((unsigned char) face < (unsigned char) FaceId::NUM_FACES);
	assert((unsigned char) layer < (unsigned char) FaceLayer::NUM_LAYERS);
	return BlockFace::get_block_face(this->faces[(unsigned char) face][(unsigned char) layer]);
}

const Block &Block::get_block(const Id id) {
	assert((unsigned char) id < (unsigned char) Id::NUM_BLOCKS);
	return blocks[(unsigned char) id];
}
