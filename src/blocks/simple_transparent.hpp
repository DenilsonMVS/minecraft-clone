
#ifndef SIMPLE_TRANSPARENT_HPP
#define SIMPLE_TRANSPARENT_HPP

#include "transparent_drawable_block.hpp"


class SimpleTransparent : public TransparentDrawableBlock {
public:
	SimpleTransparent(const BlockFace::Id face);

	void append_face_vertices(
		const glm::ivec3 &block_position,
		const FaceId face,
		std::vector<BlockFaceVertex> &vertices,
		std::vector<TransparentBlockFaceVertex> &transparent_vertices) const override;

private:
	const BlockFace &face;
};


#endif
