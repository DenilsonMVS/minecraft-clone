
#include "chunks.hpp"

#include <functional>

#include "utils.hpp"


Chunks::MainProgram::MainProgram() {
	const char *shader_source[] = {
		"resources/shaders/main.vert",
		"resources/shaders/main.frag"
	};

	new (&this->program) Program(shader_source);

	this->u_mvp = this->program.get_uniform("u_mvp");
	this->u_offset = this->program.get_uniform("u_offset");
	this->u_texture = this->program.get_uniform("u_texture");

	this->u_texture.set(0);
};

Chunks::TransparentProgram::TransparentProgram() {
	const char *shader_source[] = {
		"resources/shaders/transparent.vert",
		"resources/shaders/transparent.frag"
	};

	new (&this->program) Program(shader_source);

	this->u_mvp = this->program.get_uniform("u_mvp");
	this->u_offset = this->program.get_uniform("u_offset");
	this->u_texture = this->program.get_uniform("u_texture");
	this->u_depth = this->program.get_uniform("u_depth");
	this->u_enable_depth_test = this->program.get_uniform("u_enable_depth_test");
	this->u_window_dimensions = this->program.get_uniform("u_window_dimensions");

	this->u_texture.set(0);
	this->u_depth.set(1);
	this->u_window_dimensions.set(glm::ivec2 {800, 600});
};

Chunks::ScreenProgram::ScreenProgram() {
	const char *shader_source[] = {
		"resources/shaders/screen.vert",
		"resources/shaders/screen.frag"
	};

	new (&this->program) Program(shader_source);

	this->u_color = this->program.get_uniform("u_color");
	this->u_depth = this->program.get_uniform("u_depth");

	this->u_color.set(0);
	this->u_depth.set(1);


	struct Vertex {
		glm::vec2 position;
		glm::vec2 texture_coord;
	};

	static const std::array<LayoutElement, 2> layout = {{
		{2, gl::get_enum<float>(), false},
		{2, gl::get_enum<float>(), false}
	}};

	static const std::array<Vertex, 4> vertices = {{
        {{-1.0f, -1.0f}, {0.0f, 0.0f}},
		{{-1.0f,  1.0f}, {0.0f, 1.0f}},
        {{ 1.0f, -1.0f}, {1.0f, 0.0f}},
        {{ 1.0f,  1.0f}, {1.0f, 1.0f}}
	}};

	new (&this->buffer) SuperBuffer(layout);
	this->buffer.assign_data<Vertex>(vertices, gl::Usage::STATIC_DRAW);
}

Chunks::Chunks(const int radius, const glm::dvec3 &player_position) :
	radius(radius),
	center_chunk_position(get_chunk_pos_based_on_block_inside(det::to_int(player_position)))
{
	for(int i = 0; i < iot_num_passes; i++)
		new (&this->framebuffers[i]) Framebuffer({800, 600});

	this->generate_chunk_generation_queue();
}

Block::Id Chunks::get_block(const glm::ivec3 &block_global_position) const {
	const glm::ivec3 chunk_pos = get_chunk_pos_based_on_block_inside(block_global_position);
	
	const auto it = this->chunks.find(chunk_pos);
	if(it == this->chunks.end())
		return Block::Id::NONE;
	
	const glm::ivec3 chunk_block_pos = block_global_position - chunk_pos * chunk_size;
	return it->second.get_block_id(chunk_block_pos);
}

Chunk &Chunks::get_chunk(const glm::ivec3 &chunk_pos) {
	const auto it = this->chunks.find(chunk_pos);
	assert(it != this->chunks.end());
	return it->second;
}

const Chunk &Chunks::get_chunk(const glm::ivec3 &chunk_pos) const {
	const auto it = this->chunks.find(chunk_pos);
	assert(it != this->chunks.end());
	return it->second;
}

void Chunks::gen_chunks() {
	while(!this->chunks_to_generate.empty())
		this->gen_one_chunk();
}

void Chunks::gen_chunks(const int quantity) {
	for(int i = 0; i < quantity; i++) {
		if(this->chunks_to_generate.empty())
			return;
		
		this->gen_one_chunk();
	}
}

void Chunks::draw(const glm::mat4 &mvp, const Renderer &renderer, const Texture &block_texture) const {
	renderer.enable(gl::Capability::DEPTH_TEST);
	renderer.disable(gl::Capability::BLEND);


	block_texture.bind(0);
	this->main_program.program.bind();
	this->main_program.u_mvp.set(mvp);

	for(const auto &chunk_ptr : this->chunks_to_draw_non_transparent) {
		const glm::ivec3 relative_distance = chunk_ptr->position - this->center_chunk_position;
		this->main_program.u_offset.set(glm::vec3(relative_distance * chunk_size));
		chunk_ptr->draw_non_transparent(renderer);
	}


	assert(this->transparent_chunk_mesh_is_sorted());

	this->transparent_program.program.bind();
	this->transparent_program.u_mvp.set(mvp);

	for(int i = 0; i < iot_num_passes; i++) {
		this->framebuffers[i].bind();
		renderer.clear(gl::BitField::COLOR_BUFFER | gl::BitField::DEPTH_BUFFER);

		if(i == 0)
			this->transparent_program.u_enable_depth_test.set(0);
		else
			this->transparent_program.u_enable_depth_test.set(1);
		
		if(i != 0)
			this->framebuffers[i - 1].bind_depth_map(1);


		if(i == iot_num_passes - 1)
			renderer.enable(gl::Capability::BLEND);

		for(const auto &chunk_ptr : this->chunks_to_draw_transparent) {
			const glm::ivec3 relative_distance = chunk_ptr->position - this->center_chunk_position;
			this->transparent_program.u_offset.set(glm::vec3(relative_distance * chunk_size));
			chunk_ptr->draw_transparent(renderer);
		}
	}


	Framebuffer::unbind();

	this->screen_program.program.bind();
	this->screen_program.buffer.bind();
	
	for(int i = iot_num_passes - 1; i >= 0; i--) {
		framebuffers[i].bind_color_map(0);
		framebuffers[i].bind_depth_map(1);
		renderer.draw_quads(1);
	}
}

void Chunks::update(const glm::dvec3 &player_position) {
	const glm::ivec3 int_pos = det::to_int(player_position);
	const glm::ivec3 chunk_position = get_chunk_pos_based_on_block_inside(int_pos);
	bool need_to_select_chunks_to_draw = false;

	if(chunk_position != this->center_chunk_position) {
		this->center_chunk_position = chunk_position;
		this->generate_chunk_generation_queue();
		need_to_select_chunks_to_draw = true;
	}

	for(auto &chunk : this->chunks)
		if(chunk.second.build_buffer_if_necessary(*this))
			need_to_select_chunks_to_draw = true;

	if(need_to_select_chunks_to_draw)
		this->select_chunks_to_draw();
}

void Chunks::modify_block(const glm::ivec3 &block_global_pos, const Block::Id block_id) {
	const glm::ivec3 chunk_position = get_chunk_pos_based_on_block_inside(block_global_pos);
	const glm::ivec3 block_position_inside_chunk = block_global_pos - chunk_position * chunk_size;

	const auto it = this->chunks.find(chunk_position);
	if(it != this->chunks.end()) {
		it->second.set_block(block_position_inside_chunk, block_id);

		if(block_position_inside_chunk.x == 0) {
			const glm::ivec3 side_chunk_pos = chunk_position + BlockFace::get_block_relative_position_of_face(FaceId::WEST);
			this->mark_chunk_for_update_if_chunk_exist(side_chunk_pos);
		} else if(block_position_inside_chunk.x == chunk_size - 1) {
			const glm::ivec3 side_chunk_pos = chunk_position + BlockFace::get_block_relative_position_of_face(FaceId::EAST);
			this->mark_chunk_for_update_if_chunk_exist(side_chunk_pos);
		}

		if(block_position_inside_chunk.y == 0) {
			const glm::ivec3 side_chunk_pos = chunk_position + BlockFace::get_block_relative_position_of_face(FaceId::BOTTOM);
			this->mark_chunk_for_update_if_chunk_exist(side_chunk_pos);
		} else if(block_position_inside_chunk.y == chunk_size - 1) {
			const glm::ivec3 side_chunk_pos = chunk_position + BlockFace::get_block_relative_position_of_face(FaceId::TOP);
			this->mark_chunk_for_update_if_chunk_exist(side_chunk_pos);
		}

		if(block_position_inside_chunk.z == 0) {
			const glm::ivec3 side_chunk_pos = chunk_position + BlockFace::get_block_relative_position_of_face(FaceId::SOUTH);
			this->mark_chunk_for_update_if_chunk_exist(side_chunk_pos);
		} else if(block_position_inside_chunk.z == chunk_size - 1) {
			const glm::ivec3 side_chunk_pos = chunk_position + BlockFace::get_block_relative_position_of_face(FaceId::NORTH);
			this->mark_chunk_for_update_if_chunk_exist(side_chunk_pos);
		}
	}
}

void Chunks::gen_one_chunk() {
	assert(!this->chunks_to_generate.empty());

	const auto pos = this->chunks_to_generate.front();
	this->chunks_to_generate.pop();

	Chunk &chunk = this->chunks[pos];
	new (&chunk) Chunk(pos, this->generator);

	for(unsigned char i = 0; i < (unsigned char) FaceId::NUM_FACES; i++) {
		const glm::ivec3 side_chunk_pos = pos + BlockFace::get_block_relative_position_of_face((FaceId) i);
		const auto it = this->chunks.find(side_chunk_pos);
		if(it != this->chunks.end())
			it->second.mark_for_update();	
	}
}

Chunks::ivec3_key::ivec3_key(const glm::ivec3 &vec) :
	glm::ivec3(vec) {}

bool Chunks::ivec3_key::operator<(const ivec3_key &other) const {
	for(unsigned char i = 0; i < 3; i++)
		if((*this)[i] != other[i])
			return (*this)[i] < other[i];
	return false;
}

void Chunks::generate_chunk_generation_queue() {
	this->chunks_to_generate = std::queue<glm::ivec3>();


	for(int distance = 0; distance <= this->radius; distance++) {
		for(int i = -distance; i <= distance; i++) {
			for(int j = -distance; j <= distance; j++) {
				const glm::ivec3 pos = {i, distance, j};
				this->add_chunk_position_to_queue_if_dont_exist(this->center_chunk_position + pos);
				this->add_chunk_position_to_queue_if_dont_exist(this->center_chunk_position - pos);
			}
		}

		for(int i = -distance; i <= distance; i++) {
			for(int j = -distance + 1; j < distance; j++) {
				const glm::ivec3 pos = {i, j, distance};
				this->add_chunk_position_to_queue_if_dont_exist(this->center_chunk_position + pos);
				this->add_chunk_position_to_queue_if_dont_exist(this->center_chunk_position - pos);
			}
		}

		for(int i = -distance + 1; i < distance; i++) {
			for(int j = -distance + 1; j < distance; j++) {
				const glm::ivec3 pos = {distance, i, j};
				this->add_chunk_position_to_queue_if_dont_exist(this->center_chunk_position + pos);
				this->add_chunk_position_to_queue_if_dont_exist(this->center_chunk_position - pos);
			}
		}
	}
}

void Chunks::add_chunk_position_to_queue_if_dont_exist(const glm::ivec3 &chunk_pos) {
	const auto it = this->chunks.find(chunk_pos);
	if(it == this->chunks.end())
		this->chunks_to_generate.push(chunk_pos);
}

void Chunks::mark_chunk_for_update_if_chunk_exist(const glm::ivec3 &chunk_pos) {
	const auto it = this->chunks.find(chunk_pos);
	if(it != this->chunks.end())
		it->second.mark_for_update();
}

void Chunks::select_chunks_to_draw() {
	this->chunks_to_draw_non_transparent.clear();
	this->chunks_to_draw_transparent.clear();

	for(const auto &[position, chunk] : this->chunks) {
		const glm::ivec3 relative_distance = position - this->center_chunk_position;
		if(det::infinite_norm(relative_distance) <= this->radius) {
			if(chunk.can_draw_non_transparent_mesh())
				this->chunks_to_draw_non_transparent.push_back(&chunk);
			
			if(chunk.can_draw_transparent_mesh())
				this->chunks_to_draw_transparent.push_back(&chunk);
		}
	}

	std::sort(
		this->chunks_to_draw_transparent.begin(),
		this->chunks_to_draw_transparent.end(),
		[this](const Chunk *lhs, const Chunk *rhs) {
			return
				det::linear_norm(lhs->position - this->center_chunk_position) >
				det::linear_norm(rhs->position - this->center_chunk_position);
		}
	);
}


bool Chunks::transparent_chunk_mesh_is_sorted() const {
	return std::is_sorted(
		this->chunks_to_draw_transparent.begin(),
		this->chunks_to_draw_transparent.end(),
		[this](const Chunk *lhs, const Chunk *rhs) {
			return
				det::linear_norm(lhs->position - this->center_chunk_position) >
				det::linear_norm(rhs->position - this->center_chunk_position);
		}
	);
}
