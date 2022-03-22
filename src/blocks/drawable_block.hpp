
#ifndef DRAWABLE_BLOCK_HPP
#define DRAWABLE_BLOCK_HPP

#include "block.hpp"

#include "../block_face.hpp"
#include <vector>


class DrawableBlock : public Block {
public:
	DrawableBlock(const bool transparent, const bool solid);

	virtual void append_face_vertices(
		const glm::ivec3 &block_global_position,
		const FaceId face,
		std::vector<BlockFaceVertex> &vertices,
		std::vector<TransparentBlockFaceVertex> &transparent_vertices) const = 0;

protected:
	void build_non_transparent(
		const glm::ivec3 &block_global_position,
		const FaceId face,
		const TextureCoords &main_coords,
		const TextureCoords &secondary_coords,
		std::vector<BlockFaceVertex> &vertices) const;
};

#endif
