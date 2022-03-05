
#ifndef BLOCK_TEXTURE_ATLAS_HPP
#define BLOCK_TEXTURE_ATLAS_HPP

#include "texture.hpp"
#include "block_faces_types.hpp"


class BlockTextureAtlas {
public:
	static constexpr int resolution = 16;
	static constexpr int width = (int) BlockFaceId::NUM_FACES * resolution;
	static constexpr int height = resolution;

	BlockTextureAtlas();

	static constexpr TextureCoords get_texture_coords_of_face(const BlockFaceId face_id) {
		return {
			(float) ((int) face_id) * BlockTextureAtlas::resolution / BlockTextureAtlas::width,
			0.0f,
			(float) (((int) face_id) + 1) * BlockTextureAtlas::resolution / BlockTextureAtlas::width,
			1.0f};
	}
	

	Texture texture;
};

#endif
