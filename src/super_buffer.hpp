
#ifndef SUPER_BUFFER_HPP
#define SUPER_BUFFER_HPP

#include <glad/glad.h>
#include <span>

#include "gl.hpp"


struct LayoutElement {
	unsigned count;
	unsigned type;
	unsigned normalized;
};


class SuperBuffer {
public:

	SuperBuffer() = default;
	SuperBuffer(const std::span<const LayoutElement> &layout);

	template<typename T>
	SuperBuffer(
		const std::span<const T> &vertex,
		const gl::Usage vertex_buffer_mode,
		const std::span<const LayoutElement> &layout)
	{
		new(this) SuperBuffer(layout);
		this->assign_data(vertex, vertex_buffer_mode);
	}

	SuperBuffer(SuperBuffer &&other);
	SuperBuffer &operator=(SuperBuffer &&other);

	SuperBuffer(const SuperBuffer &other) = delete;
	SuperBuffer &operator=(const SuperBuffer &other) = delete;

	~SuperBuffer();


	template<typename T>
	void assign_data(const std::span<const T> &vertex, const gl::Usage vbo_mode = gl::Usage::DYNAMIC_DRAW) const {
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

		int buffer_size;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);

		if((int) vertex.size_bytes() > buffer_size)
			glBufferData(GL_ARRAY_BUFFER, vertex.size_bytes(), vertex.data(), (unsigned) vbo_mode);
		else
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertex.size_bytes(), vertex.data());
	}

	void bind() const;


private:
	unsigned vbo;
	unsigned vao;
};


#endif
