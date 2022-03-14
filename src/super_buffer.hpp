
#ifndef SUPER_BUFFER_HPP
#define SUPER_BUFFER_HPP

#include <glad/glad.h>
#include <vector>


struct MemoryHolder {
	template<typename T, size_t N>
	MemoryHolder(T (&data)[N]) :
		data((void *) data),
		size(N * sizeof(T)) {}

	template<typename T>
	MemoryHolder(std::vector<T> &data) :
		data(data.data()),
		size(data.size() * sizeof(T)) {}

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

	SuperBuffer() = default;

	SuperBuffer(
		const std::span<const LayoutElement> &layout,
		const unsigned binding_point) :
		count(0)
	{
		glCreateVertexArrays(1, &this->vao);
		glBindVertexArray(this->vao);

		glCreateBuffers(2, this->buffers);

		glVertexArrayElementBuffer(this->vao, this->ibo);

		unsigned vertex_size = 0;
		for(const auto &element : layout)
			vertex_size += element.count * gl::get_size(element.type);
		
		glVertexArrayVertexBuffer(this->vao, binding_point, this->vbo, 0, vertex_size);

		unsigned offset = 0;
		for(unsigned i = 0; i < layout.size(); i++) {
			const auto &element = layout[i];

			glVertexArrayAttribFormat(this->vao, i, element.count, element.type, element.normalized, offset);
			glVertexArrayAttribBinding(this->vao, i, binding_point);
			glEnableVertexArrayAttrib(this->vao, i);

			offset += element.count * gl::get_size(element.type);
		}
	}

	SuperBuffer(
		const std::span<const INDEX_TYPE> &indices,
		const gl::Usage index_buffer_mode,
		const MemoryHolder &vertex,
		const gl::Usage vertex_buffer_mode,
		const std::span<const LayoutElement> &layout,
		const unsigned binding_point) :
		count(indices.size())
	{
		glCreateVertexArrays(1, &this->vao);
		glBindVertexArray(this->vao);

		glCreateBuffers(2, this->buffers);
		
		glNamedBufferData(this->ibo, indices.size_bytes(), indices.data(), (unsigned) index_buffer_mode);
		glVertexArrayElementBuffer(this->vao, this->ibo);

		unsigned vertex_size = 0;
		for(const auto &element : layout)
			vertex_size += element.count * gl::get_size(element.type);
		
		glNamedBufferData(this->vbo, vertex.size, vertex.data, (unsigned) vertex_buffer_mode);
		glVertexArrayVertexBuffer(this->vao, binding_point, this->vbo, 0, vertex_size);
		
		unsigned offset = 0;
		for(unsigned i = 0; i < layout.size(); i++) {
			const auto &element = layout[i];

			glVertexArrayAttribFormat(this->vao, i, element.count, element.type, element.normalized, offset);
			glVertexArrayAttribBinding(this->vao, i, binding_point);
			glEnableVertexArrayAttrib(this->vao, i);

			offset += element.count * gl::get_size(element.type);
		}
	}

	SuperBuffer(SuperBuffer &&other) :
		ibo(other.ibo),
		vbo(other.vbo),
		vao(other.vao),
		count(other.count)
	{
		other.ibo = 0;
		other.vbo = 0;
		other.vao = 0;
	}

	SuperBuffer &operator=(SuperBuffer &&other) {
		if(this != &other) {
			this->ibo = other.ibo;
			this->vbo = other.vbo;
			this->vao = other.vao;
			this->count = other.count;

			other.ibo = 0;
			other.vbo = 0;
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


	void assign_data(const MemoryHolder &vertex, const gl::Usage vbo_mode = gl::Usage::DYNAMIC_DRAW) const {
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

		int buffer_size;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);

		if((int) vertex.size > buffer_size)
			glBufferData(GL_ARRAY_BUFFER, vertex.size, vertex.data, (unsigned) vbo_mode);
		else
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertex.size, vertex.data);
	}

	void assign_data(const std::span<const INDEX_TYPE> &indices, const gl::Usage ibo_mode = gl::Usage::DYNAMIC_DRAW) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);

		int buffer_size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);

		if((int) indices.size_bytes() > buffer_size)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), (unsigned) ibo_mode);
		else
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size_bytes(), indices.data());
		
		this->count = indices.size();
	}


	void draw() const {
		glBindVertexArray(this->vao);
		glDrawElements(GL_TRIANGLES, this->count, (unsigned) gl::get_enum<INDEX_TYPE>(), 0);
	}

	void draw(const int count) const {
		glBindVertexArray(this->vao);
		glDrawElements(GL_TRIANGLES, count, (unsigned) gl::get_enum<INDEX_TYPE>(), 0);
	}


//private:
	union {
		unsigned buffers[NUM_BUFFERS];
		struct {
			unsigned ibo;
			unsigned vbo;
		};
	};
	unsigned vao;
	unsigned count;
};


#endif
