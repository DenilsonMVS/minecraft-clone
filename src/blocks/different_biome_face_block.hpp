
#ifndef DIFFERENT_BIOME_FACE_BLOCK_HPP
#define DIFFERENT_BIOME_FACE_BLOCK_HPP

#include "different_face_block.hpp"


class DifferentBiomeFaceBlock : public DifferentFaceBlock {
public:
	DifferentBiomeFaceBlock(
		const BlockFace::Id main_north,
		const BlockFace::Id main_south,
		const BlockFace::Id main_east,
		const BlockFace::Id main_west,
		const BlockFace::Id main_top,
		const BlockFace::Id main_bottom,
		const BlockFace::Id secondary_north,
		const BlockFace::Id secondary_south,
		const BlockFace::Id secondary_east,
		const BlockFace::Id secondary_west,
		const BlockFace::Id secondary_top,
		const BlockFace::Id secondary_bottom);
	
	virtual void append_face_vertices(
		const glm::ivec3 &block_global_position,
		const FaceId face,
		std::vector<BlockFaceVertex> &vertices,
		std::vector<TransparentBlockFaceVertex> &transparent_vertices) const override;
	
protected:
	union {
		struct {
			const BlockFace *secondary_north;
			const BlockFace *secondary_south;
			const BlockFace *secondary_east;
			const BlockFace *secondary_west;
			const BlockFace *secondary_top;
			const BlockFace *secondary_bottom;
		};
		const BlockFace *secondary_faces[(unsigned char) FaceId::NUM_FACES];
	};
};

#endif
