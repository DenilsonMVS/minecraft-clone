
#include "block_selection.hpp"

#include "chunk.hpp"


BlockSelection::BlockSelection() :
	buffer(layout)
{
	const Shader shaders[] = {
		Shader("resources/shaders/block_slection.vert"),
		Shader("resources/shaders/block_slection.frag")
	};

	new (&this->program) Program(shaders);
	const auto u_texture = program.get_uniform("u_texture");
	u_texture.set(0);
	this->u_mvp = this->program.get_uniform("u_mvp");
	this->u_offset = this->program.get_uniform("u_offset");

	this->build_buffer();
}

void BlockSelection::draw(const glm::ivec3 &block_position, const glm::mat4 &mvp, const Renderer &renderer, const glm::dvec3 &player_pos) const {
	constexpr unsigned num_faces_in_cube = 6;

	renderer.enable(gl::Capability::BLEND);

	const glm::ivec3 center_chunk_pos = get_chunk_pos_based_on_block_inside(det::to_int(player_pos));
	const glm::ivec3 block_chunk_pos = get_chunk_pos_based_on_block_inside(block_position);
	const glm::ivec3 relative_chunk_position = block_chunk_pos - center_chunk_pos;
	const glm::ivec3 block_position_inside_chunk = block_position - block_chunk_pos * chunk_size;

	this->program.bind();
	this->u_mvp.set(mvp);
	this->u_offset.set(glm::vec3(relative_chunk_position * chunk_size + block_position_inside_chunk));

	this->buffer.bind();
	renderer.draw_quads(num_faces_in_cube);
}

void BlockSelection::build_buffer() {
	constexpr unsigned num_faces_in_cube = 6;
	
	std::vector<SelectionFaceVertex> vertices;
	vertices.reserve(num_faces_in_cube * num_vertices_per_face);

	for(unsigned char i = 0; i < (unsigned char) FaceId::NUM_FACES; i++)
		this->append_face_vertices((FaceId) i, vertices);
	
	this->buffer.assign_data<SelectionFaceVertex>(vertices, gl::Usage::STATIC_DRAW);
}

void BlockSelection::append_face_vertices(const FaceId face_id, std::vector<BlockSelection::SelectionFaceVertex> &vertices) {
	
	const auto &texture_coords = BlockFace::get_block_face(BlockFace::Id::SELECTION).coords;
	const glm::vec2 texture_position[] = {
		{texture_coords.x_min, texture_coords.y_max},
		{texture_coords.x_min, texture_coords.y_min},
		{texture_coords.x_max, texture_coords.y_max},
		{texture_coords.x_max, texture_coords.y_min}
	};

	for(unsigned char i = 0; i < num_vertices_per_face; i++) {
		const SelectionFaceVertex vertex = {
			apply_eps_to_vec(BlockFace::face_positions_helper_data[(unsigned char) face_id][i]),
			texture_position[i]
		};
		vertices.push_back(vertex);
	}
}

glm::vec3 BlockSelection::apply_eps_to_vec(const glm::vec3 &v) {
	return {
		v.x > 0.5 ? v.x + eps : v.x - eps,
		v.y > 0.5 ? v.y + eps : v.y - eps,
		v.z > 0.5 ? v.z + eps : v.z - eps
	};
}

const float BlockSelection::eps = 0.001;

const std::array<LayoutElement, 2> BlockSelection::layout = {{
	{3, gl::get_enum<float>(), false},
	{2, gl::get_enum<float>(), false}
}};
