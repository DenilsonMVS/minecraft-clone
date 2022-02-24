
#ifndef VERTEX_ARRAY_HPP
#define VERTEX_ARRAY_HPP

#include "vertex_buffer.hpp"
#include "vertex_buffer_layout.hpp"


class VertexArray {
public:

	VertexArray(const VertexBufferLayout &layout, const VertexBuffer<float> &vertex_buffer);

	VertexArray(VertexArray &&other);
	VertexArray &operator=(VertexArray &&other);

	VertexArray(const VertexArray &other) = delete;
	VertexArray &operator=(const VertexArray &other) = delete;

	~VertexArray();

	unsigned get_id() const;
	void bind() const;

private:
	unsigned id;
};

#endif
