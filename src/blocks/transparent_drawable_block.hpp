
#ifndef TRANSPARENT_DRAWABLE_BLOCK
#define TRANSPARENT_DRAWABLE_BLOCK

#include "drawable_block.hpp"

class TransparentDrawableBlock : public DrawableBlock {
public:
	TransparentDrawableBlock(const bool solid);

protected:
	void build_transparent(
		const glm::ivec3 &block_position,
		const FaceId face,
		const TextureCoords &coords,
		std::vector<TransparentBlockFaceVertex> &vertices) const;	
};

#endif
