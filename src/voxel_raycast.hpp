
#ifndef VOXEL_RAYCAST_HPP
#define VOXEL_RAYCAST_HPP

#include <glm/glm.hpp>
#include <optional>

#include "chunks.hpp"


std::optional<std::vector<glm::ivec3>> cast_ray(
	const Chunks &chunks,
	const glm::dvec3 &start,
	const glm::vec3 &facing,
	const float radius);

#endif
