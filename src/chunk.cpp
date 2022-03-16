
#include "chunk.hpp"

#include <vector>
#include <iostream>
#include "block_face.hpp"
#include "chunks.hpp"


Chunk::Chunk(const glm::ivec3 &position, WorldGenerator &generator) :
	need_update(false),
	num_faces(0),
	buffer(BlockFace::block_face_vertex_layout),
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
			
			for(; j < chunk_size; j++)
				this->blocks[i][j][k] = Block::Id::AIR;
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

void Chunk::build_buffer_if_necessary(const Chunks &chunks) {
	if(!this->need_update)
		return;
	
	std::vector<BlockFaceVertex> vertices;
	vertices.reserve(max_faces_in_chunk() * num_vertices_per_face);

	for(unsigned i = 0; i < chunk_size; i++) {
		for(unsigned j = 0; j < chunk_size; j++) {
			for(unsigned k = 0; k < chunk_size; k++) {
				const glm::ivec3 block_chunk_pos = {i, j, k};
				const glm::ivec3 block_world_pos = this->position * chunk_size + block_chunk_pos;
				
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
					if(near_block.invisible)
						block.append_face_vertices(block_world_pos, (FaceId) face_id, vertices);
				}
			}
		}
	}

	this->need_update = false;
	this->num_faces = vertices.size() / num_vertices_per_face;
	if(this->num_faces == 0)
		return;
	
	this->buffer.assign_data<BlockFaceVertex>(vertices, gl::Usage::STATIC_DRAW);
}

void Chunk::mark_for_update() {
	this->need_update = true;
}

void Chunk::draw(const IndexBuffer<unsigned> &ibo) const {
	if(this->num_faces != 0) {
		this->buffer.bind();
		ibo.draw(this->num_faces * num_indices_per_face);
	}
}
