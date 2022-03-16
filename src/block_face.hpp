
#ifndef BLOCK_FACE_HPP
#define BLOCK_FACE_HPP

#include <glm/glm.hpp>
#include <array>

#include "texture_coords.hpp"
#include "index_buffer.hpp"
#include "super_buffer.hpp"


static constexpr int num_vertices_per_face = 4;
static constexpr int num_indices_per_face = 6;


enum class FaceId : unsigned char {
	NORTH,
	SOUTH,
	EAST,
	WEST,
	TOP,
	BOTTOM,
	NUM_FACES
};

enum class FaceLayer : unsigned char {
	MAIN,
	SECONDARY,
	NUM_LAYERS,
	AIR
};

struct BlockFaceVertex {
	glm::vec3 position;
	glm::vec3 biome_color;
	glm::vec2 main_text_coord;
	glm::vec2 sec_text_coord;
	float bright;
};

class BlockFace {
public:
	const char *source;
	TextureCoords coords;

	enum class Id : unsigned char {
		AIR,
		DIRT,
		GRASS_SIDE,
		GRASS_TOP,
		STONE,
		NUM_FACES,
		NONE
	};

	static const BlockFace &get_block_face(const Id id);
	static const glm::ivec3 &get_block_relative_position_of_face(const FaceId id);
	static IndexBuffer<unsigned> gen_index_buffer_to_gen_faces(const unsigned num_faces);

	static const std::array<LayoutElement, 5> block_face_vertex_layout;

private:
	static const std::array<BlockFace, (size_t) Id::NUM_FACES> block_faces;
	static const std::array<glm::ivec3, (unsigned char) FaceId::NUM_FACES> relative_pos;
};

#endif
