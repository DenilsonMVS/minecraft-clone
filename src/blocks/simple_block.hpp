
#ifndef SIMPLE_BLOCK_HPP
#define SIMPLE_BLOCK_HPP

#include "drawable_block.hpp"


class SimpleBlock : public DrawableBlock {
public:
	SimpleBlock(const BlockFace::Id face);

	virtual void append_face_vertices(
		const glm::ivec3 &block_position,
		const FaceId face,
		std::vector<BlockFaceVertex> &vertices,
		std::vector<TransparentBlockFaceVertex> &transparent_vertices) const override;

private:
	const BlockFace &face;
};


#endif
