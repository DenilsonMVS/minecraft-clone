
#include "voxel_raycast.hpp"

#include <fixed.hpp>


typedef numeric::fixed<32, 32> fixed;
typedef glm::vec<3, fixed> fixed_vec;


static unsigned index_of_smaller(const fixed * const v, const unsigned count) {
	assert(v != nullptr);
	assert(count > 0);

	unsigned smaller = 0;
	for(unsigned i = 1; i < count; i++)
		if(v[i] < v[smaller])
			smaller = i;
	return smaller;
}

static int floor(fixed num) {
	int int_part = num.to_int();
	if(int_part > num)
		return int_part - 1;
	else
		return int_part;
}

static glm::ivec3 to_int(const fixed_vec &v) {
	return {
		floor(v.x),
		floor(v.y),
		floor(v.z)
	};
}

static glm::dvec3 to_double(const fixed_vec &v) {
	return {
		v.x.to_double(),
		v.y.to_double(),
		v.z.to_double()
	};
}

std::optional<std::vector<glm::ivec3>> cast_ray(
	const Chunks &chunks,
	const glm::dvec3 &start,
	const glm::vec3 &facing,
	const float radius)
{
	constexpr fixed eps = 1.0 / (1 << 30);

	const fixed_vec f_position = start;
	const fixed_vec f_facing = facing;
	
	std::vector<glm::ivec3> blocks_passed;
	fixed_vec current_pos = f_position;
	do {
		const glm::ivec3 block_pos = to_int(current_pos);
		blocks_passed.push_back(block_pos);

		const auto block_id = chunks.get_block(block_pos);
		if(block_id != Block::Id::NONE) {
			const auto &block = Block::get_block(block_id);
			if(block.solid)
				return blocks_passed;
		} else
			std::nullopt;


		const fixed_vec block_pos_s = block_pos;
		const fixed_vec distance = {
			facing.x < 0 ? block_pos_s.x - current_pos.x : 1 - (current_pos.x - block_pos_s.x),
			facing.y < 0 ? block_pos_s.y - current_pos.y : 1 - (current_pos.y - block_pos_s.y),
			facing.z < 0 ? block_pos_s.z - current_pos.z : 1 - (current_pos.z - block_pos_s.z)
		};
		const fixed_vec time = distance / f_facing;

		const unsigned smaller = index_of_smaller(&time[0], 3);
		current_pos += f_facing * time[smaller];
		current_pos[smaller] = facing[smaller] < 0 ?
			(block_pos[smaller] - eps) :
			(block_pos[smaller] + 1); 

	} while(glm::length(to_double(current_pos) - start) < radius);
	
	return std::nullopt;
}
