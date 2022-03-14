
#include "perlin_noise.hpp"

#include <random>
#include <algorithm>


PerlinNoise::PerlinNoise(const uint32_t seed) {
	
	for(size_t i = 0; i < permutation_size; i++)
		this->permutation_data[i] = i;

	auto engine = std::default_random_engine(seed);
	std::shuffle(permutation_data, &permutation_data[permutation_size], engine);

	for(size_t i = 0; i < permutation_size; i++)
		this->permutation_data[i + permutation_size] = this->permutation_data[i];
}

float PerlinNoise::noise(const glm::vec3 &position) const {
	
	const int32_t X = std::floor(position.x),
	              Y = std::floor(position.y),
				  Z = std::floor(position.z);
	
	const float x = position.x - X,
	            y = position.y - Y,
	            z = position.z - Z;
	
	const float u = fade(x),
	            v = fade(y),
				w = fade(z);

	const int32_t A = this->permutation_data[X    ] + Y, AA = this->permutation_data[A] + Z, AB = this->permutation_data[A + 1] + Z,
                  B = this->permutation_data[X + 1] + Y, BA = this->permutation_data[B] + Z, BB = this->permutation_data[B + 1] + Z;
	
	return lerp(w, lerp(v, lerp(u, grad(this->permutation_data[AA    ], glm::vec3(x    , y    , z    )),
                                   grad(this->permutation_data[BA    ], glm::vec3(x - 1, y    , z    ))),
                           lerp(u, grad(this->permutation_data[AB    ], glm::vec3(x    , y - 1, z    )),
                                   grad(this->permutation_data[BB    ], glm::vec3(x - 1, y - 1, z    )))),
                   lerp(v, lerp(u, grad(this->permutation_data[AA + 1], glm::vec3(x    , y    , z - 1)),
                                   grad(this->permutation_data[BA + 1], glm::vec3(x - 1, y    , z - 1))),
                           lerp(u, grad(this->permutation_data[AB + 1], glm::vec3(x    , y - 1, z - 1)),
                                   grad(this->permutation_data[BB + 1], glm::vec3(x - 1, y - 1, z - 1)))));
}

float PerlinNoise::fade(const float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::lerp(const float t, const float a, const float b) {
	return a + t * (b - a);
}

float PerlinNoise::grad(const int32_t hash, const glm::vec3 &position) {
	const uint8_t h = hash & 15;
	const float u = h < 8 ? position.x : position.y;
	const float v = h < 4 ? position.y : h == 12 || h == 14 ? position.x : position.z;
    return (((h & 1) == 0) ? u : -u) + (((h & 2) == 0) ? v : -v);
}
