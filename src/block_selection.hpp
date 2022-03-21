
#ifndef BLOCK_SELECTION_HPP
#define BLOCK_SELECTION_HPP

#include <glm/glm.hpp>
#include <vector>

#include "super_buffer.hpp"
#include "program.hpp"
#include "block_face.hpp"
#include "renderer.hpp"

class BlockSelection {
public:
	BlockSelection();

	void draw(const glm::ivec3 &block_position, const glm::mat4 &mvp, const Renderer &renderer, const glm::dvec3 &player_pos) const;

private:
	struct SelectionFaceVertex {
		glm::vec3 position;
		glm::vec2 text_coord;
	};

	void build_buffer();
	static void append_face_vertices(const FaceId face_id, std::vector<SelectionFaceVertex> &vertices);
	static glm::vec3 apply_eps_to_vec(const glm::vec3 &v);

	SuperBuffer buffer;
	Program program;
	Uniform u_mvp;
	Uniform u_offset;

	static const float eps;
	static const std::array<LayoutElement, 2> layout;
};

#endif
