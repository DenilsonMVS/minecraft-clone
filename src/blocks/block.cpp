
#include "block.hpp"


Block::Block(const bool invisible, const bool transparent, const bool solid) :
	invisible(invisible), transparent(transparent), solid(solid) {}

Block::~Block() {}

const Block &Block::get_block(const Block::Id id) {
	return *blocks[(unsigned char) id];
}


#include "simple_block.hpp"
#include "different_biome_face_block.hpp"
#include "water.hpp"

static const auto air = Block(true, true, false);
static const auto dirt = SimpleBlock(BlockFace::Id::DIRT);
static const auto grass = DifferentBiomeFaceBlock(
	BlockFace::Id::DIRT,
	BlockFace::Id::DIRT,
	BlockFace::Id::DIRT,
	BlockFace::Id::DIRT,
	BlockFace::Id::AIR,
	BlockFace::Id::DIRT,
	BlockFace::Id::GRASS_SIDE,
	BlockFace::Id::GRASS_SIDE,
	BlockFace::Id::GRASS_SIDE,
	BlockFace::Id::GRASS_SIDE,
	BlockFace::Id::GRASS_TOP,
	BlockFace::Id::AIR
);
static const auto stone = SimpleBlock(BlockFace::Id::STONE);
static const auto water = Water();

const std::array<const Block *, (size_t) Block::Id::NUM_BLOCKS> Block::blocks = {{
	&air,
	&dirt,
	&grass,
	&stone,
	&water
}};
