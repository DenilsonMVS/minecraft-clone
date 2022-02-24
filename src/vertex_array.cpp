
#include "vertex_array.hpp"


VertexArray::VertexArray(const VertexBufferLayout &layout, const VertexBuffer<float> &vertex_buffer) {
	glGenVertexArrays(1, &this->id);

	glBindVertexArray(this->id);
	vertex_buffer.bind();

	size_t offset = 0;
	const auto &elements = layout.get_elements();
	for(size_t i = 0; i < elements.size(); i++) {
		const auto &element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i,
			element.count, element.type, element.normalized,
			layout.get_stride(),
			(void *) offset);
		offset += element.count * det_gl::get_size(element.type);
	}
}

VertexArray::VertexArray(VertexArray &&other) :
	id(other.id) {
	other.id = 0;
}

VertexArray &VertexArray::operator=(VertexArray &&other) {
	if(this != &other) {
		glDeleteBuffers(1, &this->id);
		this->id = other.id;
		other.id = 0;
	}
	return (*this);
}

unsigned VertexArray::get_id() const {
	return this->id;
}

void VertexArray::bind() const {
	glBindVertexArray(this->id);
}

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &this->id);
}
