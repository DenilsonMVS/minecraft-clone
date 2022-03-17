
#ifndef PERLIN_NOISE_HPP
#define PERLIN_NOISE_HPP

#include <cstddef>
#include <array>

#include "glm/glm.hpp"


class PerlinNoise {
public:

	PerlinNoise(
		const float frequency = 1,
		const float amplitude = 1,
		const float lacunarity = 2,
		const float persistence = 0.5,
		const uint32_t seed = 0);

	float noise(float x) const;
	float noise(float x, float y) const;
	float noise(float x, float y, float z) const;
	
	float noise(const glm::vec1 &pos) const;
	float noise(const glm::vec2 &pos) const;
	float noise(const glm::vec3 &pos) const;

	float fractal(size_t octaves, float x) const;
    float fractal(size_t octaves, float x, float y) const;
    float fractal(size_t octaves, float x, float y, float z) const;

	float fractal(size_t octaves, const glm::vec1 &pos) const;
	float fractal(size_t octaves, const glm::vec2 &pos) const;
	float fractal(size_t octaves, const glm::vec3 &pos) const;


	// Parameters of Fractional Brownian Motion (fBm) : sum of N "octaves" of noise
    float frequency;   ///< Frequency ("width") of the first octave of noise (default to 1.0)
    float amplitude;   ///< Amplitude ("height") of the first octave of noise (default to 1.0)
    float lacunarity;  ///< Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
    float persistence; ///< Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)

private:

	inline uint8_t hash(int32_t i) const;

	static inline int32_t fastfloor(float fp);

	static float grad(int32_t hash, float x);
	static float grad(int32_t hash, float x, float y);
	static float grad(int32_t hash, float x, float y, float z);

	static constexpr size_t permutation_size = 256;
	std::array<uint8_t, permutation_size> perm;
};

#endif
