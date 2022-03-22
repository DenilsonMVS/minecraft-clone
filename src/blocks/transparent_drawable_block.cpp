
#include "transparent_drawable_block.hpp"


TransparentDrawableBlock::TransparentDrawableBlock(const bool solid) :
	DrawableBlock(true, solid) {}

void TransparentDrawableBlock::build_transparent(
	const glm::ivec3 &block_position,
	const FaceId face,
	const TextureCoords &coords,
	std::vector<TransparentBlockFaceVertex> &vertices) const
{
	const glm::vec2 vertices_coords[] = {
		{coords.x_min, coords.y_max},
		{coords.x_min, coords.y_min},
		{coords.x_max, coords.y_max},
		{coords.x_max, coords.y_min}
	};

	for(int i = 0; i < num_vertices_per_face; i++) {
		const TransparentBlockFaceVertex vertice = {
			block_position + BlockFace::face_positions_helper_data[(unsigned char) face][i],
			vertices_coords[i],
			0.8
		};
		vertices.push_back(vertice);
	}
}
