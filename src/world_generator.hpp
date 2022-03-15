
#ifndef WORLD_GENERATOR_HPP
#define WORLD_GENERATOR_HPP

#include <glm/glm.hpp>
#include <map>

#include "chunk_definitions.hpp"
#include "perlin_noise.hpp"


class WorldGenerator {
public:
	int get_height(const glm::ivec2 &block_global_pos);

private:
	struct WorldGeneratorChunk {
		int height[chunk_size][chunk_size];
	};

	struct ivec2_key : public glm::ivec2 {
		ivec2_key(const glm::ivec2 &vec);
		bool operator<(const ivec2_key &other) const;
	};


	const WorldGeneratorChunk &generate_chunk(const glm::ivec2 &chunk_pos);
	int noise_generator_get_height(const glm::ivec2 &block_global_pos);

	std::map<ivec2_key, WorldGeneratorChunk> chunks;
	PerlinNoise noise_generator;
};


#endif
