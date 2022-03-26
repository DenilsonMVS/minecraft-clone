
#include "chunk.hpp"

#include <vector>
#include <iostream>

#include "block_face.hpp"
#include "chunks.hpp"


Chunk::Chunk(const glm::ivec3 &position, WorldGenerator &generator) :
	need_update(true),
	num_faces(0),
	transparent_faces(0),
	main_buffer(BlockFace::block_face_vertex_layout),
	transparent_buffer(BlockFace::transparent_block_face_vertex_layout),
	position(position)
{
	const glm::ivec2 pos2 = {this->position.x, this->position.z};
	for(int i = 0; i < chunk_size; i++) {
		for(int k = 0; k < chunk_size; k++) {
			const glm::ivec2 block_chunk_pos = {i, k};
			const glm::ivec2 block_global_pos = pos2 * chunk_size + block_chunk_pos;
			
			const int height = generator.get_height(block_global_pos);
			const int block_chunk_height = height - this->position.y * chunk_size;
			
			int j = 0;
			for(; j < (block_chunk_height - 3) && j < chunk_size; j++)
				this->blocks[i][j][k] = Block::Id::STONE;
			
			for(; j < block_chunk_height && j < chunk_size; j++)
				this->blocks[i][j][k] = Block::Id::DIRT;
			
			if(block_chunk_height >= 0 && block_chunk_height < chunk_size)
				this->blocks[i][j++][k] = Block::Id::GRASS;
			
			if(position.y >= 0)
				for(; j < chunk_size; j++)
					this->blocks[i][j][k] = Block::Id::AIR;
			else
				for(; j < chunk_size; j++)
					this->blocks[i][j][k] = Block::Id::WATER;
		}
	}
}

Block::Id Chunk::get_block_id(const glm::ivec3 &block_position_in_chunk) const {
	if(
		block_position_in_chunk.x < 0 || block_position_in_chunk.x >= chunk_size ||
		block_position_in_chunk.y < 0 || block_position_in_chunk.y >= chunk_size ||
		block_position_in_chunk.z < 0 || block_position_in_chunk.z >= chunk_size
	)
		return Block::Id::NONE;
	return this->blocks[block_position_in_chunk.x][block_position_in_chunk.y][block_position_in_chunk.z];
}

void Chunk::set_block(const glm::ivec3 &block_position_in_chunk, const Block::Id block_id) {
	assert(block_position_in_chunk.x >= 0 && block_position_in_chunk.x < chunk_size);
	assert(block_position_in_chunk.y >= 0 && block_position_in_chunk.y < chunk_size);
	assert(block_position_in_chunk.z >= 0 && block_position_in_chunk.z < chunk_size);

	this->blocks[block_position_in_chunk.x][block_position_in_chunk.y][block_position_in_chunk.z] = block_id;
	this->mark_for_update();
}

bool Chunk::build_buffer_if_necessary(const Chunks &chunks) {
	if(!this->need_update)
		return false;
	this->need_update = false;

	std::vector<BlockFaceVertex> vertices;
	vertices.reserve(max_faces_in_chunk() * num_vertices_per_face);
	std::vector<TransparentBlockFaceVertex> transparent_vertices;

	for(unsigned i = 0; i < chunk_size; i++) {
		for(unsigned j = 0; j < chunk_size; j++) {
			for(unsigned k = 0; k < chunk_size; k++) {
				const glm::ivec3 block_chunk_pos = {i, j, k};
				
				const Block::Id block_id = this->get_block_id(block_chunk_pos);
				const Block &block = Block::get_block(block_id);
				if(block.invisible)
					continue;

				for(unsigned char face_id = 0; face_id < (unsigned char) FaceId::NUM_FACES; face_id++) {
					const glm::ivec3 near_block_chunk_pos = block_chunk_pos + BlockFace::get_block_relative_position_of_face((FaceId) face_id);
					Block::Id near_block_id = this->get_block_id(near_block_chunk_pos);
					if(near_block_id == Block::Id::NONE) {
						const glm::ivec3 near_block_global_pos = this->position * chunk_size + near_block_chunk_pos;
						near_block_id = chunks.get_block(near_block_global_pos);

						if(near_block_id == Block::Id::NONE)
							continue;
					}
					
					const Block &near_block = Block::get_block(near_block_id);
					if(near_block.transparent && (block_id != near_block_id)) {
						const DrawableBlock &drawable_block = (const DrawableBlock &) block;
						drawable_block.append_face_vertices(block_chunk_pos, (FaceId) face_id, vertices, transparent_vertices);
					}
				}
			}
		}
	}


	this->num_faces = vertices.size() / num_vertices_per_face;
	if(this->num_faces != 0)
		this->main_buffer.assign_data<BlockFaceVertex>(vertices);
	
	this->transparent_faces = transparent_vertices.size() / num_vertices_per_face;
	if(this->transparent_faces != 0)
		this->transparent_buffer.assign_data<TransparentBlockFaceVertex>(transparent_vertices);
	
	return this->num_faces != 0 || this->transparent_faces != 0;
}

/*bool Chunk::build_only_non_transparent_buffer(const Chunks &chunks) {
	if(!this->need_update)
		return false;
	this->need_update = false;

	std::vector<BlockFaceVertex> vertices;
	vertices.reserve(max_faces_in_chunk() * num_vertices_per_face);
	std::vector<TransparentBlockFaceVertex> transparent_vertices;

	for(unsigned i = 0; i < chunk_size; i++) {
		for(unsigned j = 0; j < chunk_size; j++) {
			for(unsigned k = 0; k < chunk_size; k++) {
				const glm::ivec3 block_chunk_pos = {i, j, k};
				
				const Block::Id block_id = this->get_block_id(block_chunk_pos);
				const Block &block = Block::get_block(block_id);
				if(block.transparent)
					continue;

				for(unsigned char face_id = 0; face_id < (unsigned char) FaceId::NUM_FACES; face_id++) {
					const glm::ivec3 near_block_chunk_pos = block_chunk_pos + BlockFace::get_block_relative_position_of_face((FaceId) face_id);
					Block::Id near_block_id = this->get_block_id(near_block_chunk_pos);
					if(near_block_id == Block::Id::NONE) {
						const glm::ivec3 near_block_global_pos = this->position * chunk_size + near_block_chunk_pos;
						near_block_id = chunks.get_block(near_block_global_pos);

						if(near_block_id == Block::Id::NONE)
							continue;
					}
					
					const Block &near_block = Block::get_block(near_block_id);
					if(near_block.transparent) {
						const DrawableBlock &drawable_block = (const DrawableBlock &) block;
						drawable_block.append_face_vertices(block_chunk_pos, (FaceId) face_id, vertices, transparent_vertices);
					}
				}
			}
		}
	}

	this->num_faces = vertices.size() / num_vertices_per_face;
	if(this->num_faces != 0)
		this->main_buffer.assign_data<BlockFaceVertex>(vertices);
	
	return this->num_faces != 0;
}

bool Chunk::transparent_buffer_sort(const Chunks &chunks, const glm::ivec3 &central_block) {

	std::array<glm::ivec3, chunk_size * chunk_size * chunk_size> transparent_block_positions;
	size_t num_blocks = 0;

	for(unsigned i = 0; i < chunk_size; i++) {
		for(unsigned j = 0; j < chunk_size; j++) {
			for(unsigned k = 0; k < chunk_size; k++) {
				const glm::ivec3 block_chunk_pos = {i, j, k};

				const Block::Id block_id = this->get_block_id(block_chunk_pos);
				const Block &block = Block::get_block(block_id);
				if(block.invisible)
					continue;
				
				for(unsigned char face_id = 0; face_id < (unsigned char) FaceId::NUM_FACES; face_id++) {
					const glm::ivec3 near_block_chunk_pos = block_chunk_pos + BlockFace::get_block_relative_position_of_face((FaceId) face_id);
					Block::Id near_block_id = this->get_block_id(near_block_chunk_pos);
					if(near_block_id == Block::Id::NONE) {
						const glm::ivec3 near_block_global_pos = this->position * chunk_size + near_block_chunk_pos;
						near_block_id = chunks.get_block(near_block_global_pos);

						if(near_block_id == Block::Id::NONE)
							continue;
					}
					
					const Block &near_block = Block::get_block(near_block_id);
					if(near_block.transparent && (block_id != near_block_id)) {
						transparent_block_positions[num_blocks++] = block_chunk_pos;
						break;
					}
				}
			}
		}
	}


	std::sort(
		&transparent_block_positions[0],
		&transparent_block_positions[1],
		[&central_block](const glm::ivec3 &lhs, const glm::ivec3 &rhs) {
			return
				det::linear_norm(lhs - central_block) <
				det::linear_norm(rhs - central_block);
		});


	std::vector<BlockFaceVertex> vertices;
	std::vector<TransparentBlockFaceVertex> transparent_vertices;
	transparent_vertices.reserve(num_blocks * (size_t) FaceId::NUM_FACES);

	for(size_t i = 0; i < num_blocks; i++) {
		const glm::ivec3 block_chunk_pos = transparent_block_positions[i];
				
		const Block::Id block_id = this->get_block_id(block_chunk_pos);
		const Block &block = Block::get_block(block_id);
		if(block.invisible)
			continue;

		for(unsigned char face_id = 0; face_id < (unsigned char) FaceId::NUM_FACES; face_id++) {
			const glm::ivec3 near_block_chunk_pos = block_chunk_pos + BlockFace::get_block_relative_position_of_face((FaceId) face_id);
			Block::Id near_block_id = this->get_block_id(near_block_chunk_pos);
			if(near_block_id == Block::Id::NONE) {
				const glm::ivec3 near_block_global_pos = this->position * chunk_size + near_block_chunk_pos;
				near_block_id = chunks.get_block(near_block_global_pos);

				if(near_block_id == Block::Id::NONE)
					continue;
			}
			
			const Block &near_block = Block::get_block(near_block_id);
			if(near_block.transparent && (block_id != near_block_id)) {
				const DrawableBlock &drawable_block = (const DrawableBlock &) block;
				drawable_block.append_face_vertices(block_chunk_pos, (FaceId) face_id, vertices, transparent_vertices);
			}
		}
	}


	this->transparent_faces = transparent_vertices.size() / num_vertices_per_face;
	if(this->transparent_faces != 0) {
		this->transparent_buffer.assign_data<TransparentBlockFaceVertex>(transparent_vertices);
		return true;
	} else
		return false;
}*/

void Chunk::mark_for_update() {
	this->need_update = true;
}

void Chunk::draw_non_transparent(const Renderer &renderer) const {
	if(this->num_faces != 0) {
		this->main_buffer.bind();
		renderer.draw_quads(this->num_faces);
	}
}

void Chunk::draw_transparent(const Renderer &renderer) const {
	if(this->transparent_faces != 0) {
		this->transparent_buffer.bind();
		renderer.draw_quads(this->transparent_faces);
	}
}

bool Chunk::can_draw_non_transparent_mesh() const {
	return this->num_faces != 0;
}

bool Chunk::can_draw_transparent_mesh() const {
	return this->transparent_faces != 0;
}

glm::vec3 Chunk::offset_to_draw(const glm::ivec3 &block_world_pos, const glm::vec3 &player_pos) {
	const glm::ivec3 center_chunk_pos = get_chunk_pos_based_on_block_inside(det::to_int(player_pos));
	const glm::ivec3 block_chunk_pos = get_chunk_pos_based_on_block_inside(block_world_pos);
	const glm::ivec3 relative_chunk_position = block_chunk_pos - center_chunk_pos;
	return glm::vec3(relative_chunk_position * chunk_size);
}
