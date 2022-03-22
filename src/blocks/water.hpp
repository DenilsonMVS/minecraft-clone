
#ifndef WATER_HPP
#define WATER_HPP

#include "drawable_block.hpp"


class Water : public DrawableBlock {
public:
	Water();

	virtual void append_face_vertices(
		const glm::ivec3 &block_global_position,
		const FaceId face,
		std::vector<BlockFaceVertex> &vertices,
		std::vector<TransparentBlockFaceVertex> &transparent_vertices) const override;

private:
	const BlockFace &face;
};


#endif
