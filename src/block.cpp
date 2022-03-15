
#include "block.hpp"

#include <cassert>


const Block Block::blocks[(unsigned char) Id::NUM_BLOCKS] = {
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
};

const Block &Block::get_block(const Block::Id id) {
	assert(id != Block::Id::NONE);
	return blocks[(unsigned char) id];
}

const BlockFace &Block::get_block_face(const Block::Id id, const FaceId face, const FaceLayer layer) {
	return BlockFace::block_faces[(unsigned char) get_block(id).faces[(unsigned char) face][(unsigned char) layer]];
}
const TextureCoords &Block::get_coords(const Block::Id id, const FaceId face, const FaceLayer layer) {
	return get_block_face(id, face, layer).coords;
}
