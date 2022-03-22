
#ifndef BIOME_BLOCK_HPP
#define BIOME_BLOCK_HPP

#include "drawable_block.hpp"


class BiomeBlock : public DrawableBlock {
public:
	BiomeBlock(const BlockFace::Id main_face, const BlockFace::Id secondary_face);

	virtual void append_face_vertices(
		const glm::ivec3 &block_global_position,
		const FaceId face,
		std::vector<BlockFaceVertex> &vertices,
		std::vector<TransparentBlockFaceVertex> &transparent_vertices) const override;

private:
	union {
		struct {
			const BlockFace *main_face;
			const BlockFace *secondary_face;
		};
		const BlockFace *faces[(unsigned char) FaceLayer::NUM_LAYERS];
	};
	
};

#endif
