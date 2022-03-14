
#ifndef PERLIN_NOISE_HPP
#define PERLIN_NOISE_HPP

#include <cstddef>
#include <cstdint>

#include "glm/glm.hpp"


class PerlinNoise {
public:

	PerlinNoise(const uint32_t seed = 0);

	float noise(const glm::vec3 &position) const;

private:

	static float fade(const float t);
	static float lerp(const float t, const float a, const float b);
	static float grad(const int32_t hash, const glm::vec3 &position);

	static constexpr size_t permutation_size = 256;
	uint8_t permutation_data[permutation_size * 2];
};

#endif
