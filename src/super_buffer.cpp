
#include "super_buffer.hpp"


SuperBuffer::SuperBuffer(const std::span<const LayoutElement> &layout) {
	glCreateVertexArrays(1, &this->vao);
	glCreateBuffers(1, &this->vbo);
	
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	unsigned vertex_size = 0;
	for(const auto &element : layout)
		vertex_size += element.count * gl::get_size((unsigned) element.type);

	size_t offset = 0;
	for(unsigned i = 0; i < layout.size(); i++) {
		const auto &element = layout[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i,
			element.count, (unsigned) element.type, element.normalized,
			vertex_size,
			(const void *) offset);
		offset += element.count * gl::get_size((unsigned) element.type);
	}
}

SuperBuffer::SuperBuffer(SuperBuffer &&other) :
	vbo(other.vbo),
	vao(other.vao)
{
	other.vbo = 0;
	other.vao = 0;
}

SuperBuffer &SuperBuffer::operator=(SuperBuffer &&other) {
	if(this != &other) {
		this->vbo = other.vbo;
		this->vao = other.vao;

		other.vbo = 0;
		other.vao = 0;
	}

	return *this;
}

SuperBuffer::~SuperBuffer() {
	glDeleteVertexArrays(1, &this->vao);
	glDeleteBuffers(1, &this->vbo);
}

void SuperBuffer::bind() const {
	glBindVertexArray(this->vao);
}
