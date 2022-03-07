
#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "block_faces.hpp"


enum class BlockId : unsigned char {
	AIR,
	DIRT,
	GRASS,
	NUM_BLOCKS,
	NONE
};

enum class FaceLayer : unsigned char {
	MAIN,
	SECONDARY,
	NUM_LAYERS,
	AIR
};

struct BlockData {
	bool invisible: 1;
	BlockFaceId faces[(unsigned char) FaceId::NUM_FACES][(unsigned char) FaceLayer::NUM_LAYERS];
};

constexpr BlockData blocks[] = {
	{true, {
		{BlockFaceId::AIR, BlockFaceId::AIR},
		{BlockFaceId::AIR, BlockFaceId::AIR},
		{BlockFaceId::AIR, BlockFaceId::AIR},
		{BlockFaceId::AIR, BlockFaceId::AIR},
		{BlockFaceId::AIR, BlockFaceId::AIR},
		{BlockFaceId::AIR, BlockFaceId::AIR}}},
	{false, {
		{BlockFaceId::DIRT, BlockFaceId::AIR},
		{BlockFaceId::DIRT, BlockFaceId::AIR},
		{BlockFaceId::DIRT, BlockFaceId::AIR},
		{BlockFaceId::DIRT, BlockFaceId::AIR},
		{BlockFaceId::DIRT, BlockFaceId::AIR},
		{BlockFaceId::DIRT, BlockFaceId::AIR}}},
	{false, {
		{BlockFaceId::DIRT, BlockFaceId::GRASS_SIDE},
		{BlockFaceId::DIRT, BlockFaceId::GRASS_SIDE}, 
		{BlockFaceId::DIRT, BlockFaceId::GRASS_SIDE}, 
		{BlockFaceId::DIRT, BlockFaceId::GRASS_SIDE}, 
		{BlockFaceId::AIR, BlockFaceId::GRASS_TOP}, 
		{BlockFaceId::DIRT, BlockFaceId::AIR}}}
};

const BlockData &get_block(const BlockId id);
const FaceData &get_block_face(const BlockId id, const FaceId face, const FaceLayer layer);
const TextureCoords &get_coords(const BlockId id, const FaceId face, const FaceLayer layer);

#endif
