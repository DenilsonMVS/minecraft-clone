
#include "vertex_buffer_layout.hpp"

#include "utils.hpp"


VertexBufferLayout::VertexBufferLayout() :
	elements(std::vector<VertexBufferElement>()), stride(0) {}

void VertexBufferLayout::push(const int count, const unsigned type, const int normalized) {
	const VertexBufferElement element = {
		count,
		type,
		normalized};

	this->elements.push_back(element);
	this->stride += count * sizeof(det_gl::get_size(type));
}

unsigned VertexBufferLayout::get_stride() const {
	return this->stride;
}

const std::vector<VertexBufferLayout::VertexBufferElement> &VertexBufferLayout::get_elements() const {
	return this->elements;
}
