
#include "water.hpp"


Water::Water() :
	DrawableBlock(true, false),
	face(BlockFace::get_block_face(BlockFace::Id::WATER)) {}

void Water::append_face_vertices(
	const glm::ivec3 &block_global_position,
	const FaceId face,
	std::vector<BlockFaceVertex> &vertices,
	std::vector<TransparentBlockFaceVertex> &transparent_vertices) const
{
	const auto &text_positions = this->face.coords;
	const glm::vec2 vbo_text_positions[] = {
		{text_positions.x_min, text_positions.y_max},
		{text_positions.x_min, text_positions.y_min},
		{text_positions.x_max, text_positions.y_max},
		{text_positions.x_max, text_positions.y_min}
	};

	for(int i = 0; i < num_vertices_per_face; i++) {
		const TransparentBlockFaceVertex vertex = {
			block_global_position + BlockFace::face_positions_helper_data[(unsigned char) face][i],
			vbo_text_positions[i],
			0.8
		};
		transparent_vertices.push_back(vertex);
	}
}
