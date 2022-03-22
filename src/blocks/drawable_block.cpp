
#include "drawable_block.hpp"


DrawableBlock::DrawableBlock(const bool transparent, const bool solid) :
	Block(false, transparent, solid) {}

void DrawableBlock::build_non_transparent(
	const glm::ivec3 &block_global_position,
	const FaceId face,
	const TextureCoords &main_coords,
	const TextureCoords &secondary_coords,
	std::vector<BlockFaceVertex> &vertices) const
{
	const glm::vec2 vbo_main_text_positions[] = {
		{main_coords.x_min, main_coords.y_max},
		{main_coords.x_min, main_coords.y_min},
		{main_coords.x_max, main_coords.y_max},
		{main_coords.x_max, main_coords.y_min}
	};

	const glm::vec2 vbo_sec_text_positions[] = {
		{secondary_coords.x_min, secondary_coords.y_max},
		{secondary_coords.x_min, secondary_coords.y_min},
		{secondary_coords.x_max, secondary_coords.y_max},
		{secondary_coords.x_max, secondary_coords.y_min}
	};
	
	for(int i = 0; i < num_vertices_per_face; i++) {
		const BlockFaceVertex vertex = {
			block_global_position + BlockFace::face_positions_helper_data[(unsigned char) face][i],
			{0.1, 0.7, 0.15},
			vbo_main_text_positions[i],
			vbo_sec_text_positions[i],
			0.8};
		vertices.push_back(vertex);
	}
}
