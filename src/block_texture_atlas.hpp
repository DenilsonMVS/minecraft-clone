
#ifndef BLOCK_TEXTURE_ATLAS_HPP
#define BLOCK_TEXTURE_ATLAS_HPP

#include "texture_coords.hpp"
#include "texture.hpp"
#include "block_face.hpp"


class BlockTextureAtlas {
public:

	BlockTextureAtlas();
	static TextureCoords get_texture_coords_of_face(const BlockFace::Id id);

	Texture texture;

	static const int resolution;
	static const int width;
	static const int height;
};

#endif
