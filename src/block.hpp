
#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "block_faces.hpp"


enum BlockId : unsigned char {
	AIR,
	DIRT,
	GRASS,
	NUM_BLOCKS
};

struct BlockData {
	bool invisible: 1;
	BlockFaceId faces[(unsigned char) FaceId::NUM_FACES];
};

constexpr BlockData blocks[] = {
	{true, {BlockFaceId::AIR, BlockFaceId::AIR, BlockFaceId::AIR, BlockFaceId::AIR, BlockFaceId::AIR, BlockFaceId::AIR}},
	{false, {BlockFaceId::DIRT, BlockFaceId::DIRT, BlockFaceId::DIRT, BlockFaceId::DIRT, BlockFaceId::DIRT, BlockFaceId::DIRT}},
	{false, {BlockFaceId::GRASS_SIDE, BlockFaceId::GRASS_SIDE, BlockFaceId::GRASS_SIDE, BlockFaceId::GRASS_SIDE, BlockFaceId::GRASS_TOP, BlockFaceId::DIRT}}
};

const BlockData &get_block(const BlockId id);
const FaceData &get_block_face(const BlockId id, const FaceId face);
const TextureCoords &get_coords(const BlockId id, const FaceId face);

#endif
