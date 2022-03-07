
#include "block_texture_atlas.hpp"

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <stb/stb_image.h>

#include "block_faces.hpp"


BlockTextureAtlas::BlockTextureAtlas() {
	typedef glm::u8vec4 pixel;


	const std::string textures_path = "resources/images/";

	constexpr int channels = 4;
	auto texture_join = std::unique_ptr<pixel []>(new pixel[width * height]);
	
	int image_index = 0;
	for(unsigned char face_id = 0; face_id < (unsigned char) BlockFaceId::NUM_FACES; face_id++) {
		const auto file_path = textures_path + block_faces[face_id].source;

		int rtn_width, rtn_height, rtn_channels;
		pixel *image = (pixel *) stbi_load(file_path.c_str(), &rtn_width, &rtn_height, &rtn_channels, channels);
		assert(image != nullptr && rtn_width == resolution && rtn_height == resolution && rtn_channels == channels);

		for(int line = 0; line < resolution; line++) {
			for(int collum = 0; collum < resolution; collum++) {
				const int part_index = line * resolution + collum;
				const int join_index = line * width + collum + image_index * resolution;

				texture_join[join_index] = image[part_index];
			}
		}

		image_index++;
		stbi_image_free(image);
	}

	new(&this->texture) Texture((unsigned char *) texture_join.get(), width, height, channels, Texture::NEAREST);
}
