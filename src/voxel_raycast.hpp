
#ifndef VOXEL_RAYCAST_HPP
#define VOXEL_RAYCAST_HPP

#include <optional>
#include <glm/glm.hpp>

#include "chunks.hpp"


std::optional<glm::ivec3> cast_ray(
	const Chunks &chunks,
	const glm::dvec3 &start,
	const glm::vec3 &facing,
	const float radius);

#endif
