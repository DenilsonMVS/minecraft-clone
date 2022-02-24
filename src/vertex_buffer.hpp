
#ifndef VERTEX_BUFFER_HPP
#define VERTEX_BUFFER_HPP

#include <span>
#include "utils.hpp"


template<typename T>
class VertexBuffer {
public:

	VertexBuffer() :
		count(0)
	{
		glGenBuffers(1, &this->id);
	}
	
	VertexBuffer(const std::span<const T> &data, const unsigned mode) :
		count(data.size())
	{
		glGenBuffers(1, &this->id);
		glBindBuffer(GL_ARRAY_BUFFER, this->id);
		glBufferData(GL_ARRAY_BUFFER, this->count * sizeof(T), data.data(), mode);
	}

	VertexBuffer(VertexBuffer &&other) :
		id(other.id), count(other.count)
	{
		other.id = 0;
	}

	VertexBuffer &operator=(VertexBuffer &&other) {
		if(this != &other) {
			glDeleteBuffers(1, &this->id);
			this->id = other.id;
			other.id = 0;

			this->count = other.count;
		}
		return *this;
	}

	VertexBuffer(const VertexBuffer &other) = delete;
	VertexBuffer &operator=(const VertexBuffer &other) = delete;

	~VertexBuffer() {
		glDeleteBuffers(1, &this->id);
	}


	unsigned get_id() const {
		return this->id;
	}

	unsigned get_count() const {
		return this->count;
	}

	int get_capacity() const {
		glBindBuffer(GL_ARRAY_BUFFER, this->id);
		int buffer_size;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
		return buffer_size / sizeof(T);
	}

	void reserve(const int minimum_capacity, const unsigned mode = GL_DYNAMIC_DRAW) {
		glBindBuffer(GL_ARRAY_BUFFER, this->id);

		int buffer_size;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);

		if(minimum_capacity * sizeof(T) > buffer_size)
			glBufferData(GL_ARRAY_BUFFER, minimum_capacity * sizeof(T), nullptr, mode);
	}

	void bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, this->id);
	}

	void assign_data(const std::span<const T> &data, const unsigned mode = GL_DYNAMIC_DRAW) {
		glBindBuffer(GL_ARRAY_BUFFER, this->id);

		int buffer_size;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);

		if(data.size() * sizeof(T) > buffer_size)
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), mode);
		else
			glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(T), data.data());
	}

	
	static constexpr unsigned get_type() {
		return det_gl::get_gl_enum<T>();
	}

private:
	unsigned id;
	unsigned count;
};




#endif
