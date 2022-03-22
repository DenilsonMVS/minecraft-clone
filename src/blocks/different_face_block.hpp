
#ifndef DIFFERENT_FACE_BLOCK
#define DIFFERENT_FACE_BLOCK

#include "drawable_block.hpp"


class DifferentFaceBlock : public DrawableBlock {
public:
	DifferentFaceBlock(
		const BlockFace::Id north,
		const BlockFace::Id south,
		const BlockFace::Id east,
		const BlockFace::Id west,
		const BlockFace::Id top,
		const BlockFace::Id bottom);
	
	virtual void append_face_vertices(
		const glm::ivec3 &block_position,
		const FaceId face,
		std::vector<BlockFaceVertex> &vertices,
		std::vector<TransparentBlockFaceVertex> &transparent_vertices) const override;

protected:
	union {
		struct {
			const BlockFace *north;
			const BlockFace *south;
			const BlockFace *east;
			const BlockFace *west;
			const BlockFace *top;
			const BlockFace *bottom;
		};
		const BlockFace *faces[(unsigned char) FaceId::NUM_FACES];
	};
};


#endif
