
#ifndef SUPER_BUFFER_HPP
#define SUPER_BUFFER_HPP

#include <glad/glad.h>


struct MemoryHolder {
	template<typename T, size_t N>
	MemoryHolder(T (&data)[N]) :
		data((void *) data),
		size(N * sizeof(T)) {}

	void *data;
	size_t size;
};

struct LayoutElement {
	unsigned count;
	unsigned type;
	unsigned normalized;
};

template<typename INDEX_TYPE>
class SuperBuffer {
	enum {
		INDEX_BUFFER,
		VERTEX_BUFFER,
		NUM_BUFFERS
	};

public:

	SuperBuffer(
		const std::span<const INDEX_TYPE> &indices,
		const unsigned index_buffer_mode,
		const MemoryHolder &vertex,
		const unsigned vertex_buffer_mode,
		const std::span<const LayoutElement> &layout,
		const unsigned binding_point) :
		binding_point(binding_point),
		count(indices.size())
	{
		glCreateVertexArrays(1, &this->vao);
		glBindVertexArray(this->vao);

		glCreateBuffers(2, this->buffers);
		
		glNamedBufferData(this->buffers[INDEX_BUFFER], indices.size() * sizeof(INDEX_TYPE), indices.data(), index_buffer_mode);
		glVertexArrayElementBuffer(this->vao, this->buffers[INDEX_BUFFER]);

		glNamedBufferData(this->buffers[VERTEX_BUFFER], vertex.size, vertex.data, vertex_buffer_mode);
		glVertexArrayVertexBuffer(this->vao, this->binding_point, this->buffers[VERTEX_BUFFER], 0, sizeof(float) * 4);
		
		unsigned offset = 0;
		for(unsigned i = 0; i < layout.size(); i++) {
			const auto element = layout[i];

			glVertexArrayAttribFormat(this->vao, i, element.count, element.type, element.normalized, offset);
			glVertexArrayAttribBinding(this->vao, i, this->binding_point);
			glEnableVertexArrayAttrib(this->vao, i);

			offset += element.count * gl::get_size(element.type);
		}
	}

	SuperBuffer(SuperBuffer &&other) :
		buffers{other.buffers[INDEX_BUFFER], other.buffers[VERTEX_BUFFER]}, 
		vao(other.vao), 
		binding_point(other.binding_point), 
		count(other.count)
	{
		other.buffers[INDEX_BUFFER] = 0;
		other.buffers[VERTEX_BUFFER] = 0;
		other.vao = 0;
	}

	SuperBuffer &operator=(SuperBuffer &&other) {
		if(this != &other) {
			this->buffers = other.buffers;
			this->vao = other.vao;
			this->binding_point = other.binding_point;
			this->count = other.count;

			other.buffers[INDEX_BUFFER] = 0;
			other.buffers[VERTEX_BUFFER] = 0;
			other.vao = 0;
		}

		return *this;
	}

	SuperBuffer(const SuperBuffer &other) = delete;
	SuperBuffer &operator=(const SuperBuffer &other) = delete;

	~SuperBuffer() {
		glDeleteVertexArrays(1, &this->vao);
		glDeleteBuffers(2, this->buffers);
	}


	void draw() const {
		glBindVertexArray(this->vao);
		glDrawElements(GL_TRIANGLES, this->count, gl::get_gl_enum<INDEX_TYPE>(), 0);
	}

	void draw(const int count) const {
		glBindVertexArray(this->vao);
		glDrawElements(GL_TRIANGLES, count, gl::get_gl_enum<INDEX_TYPE>(), 0);
	}

private:
	unsigned buffers[NUM_BUFFERS];
	unsigned vao;
	unsigned binding_point;
	unsigned count;
};


#endif
