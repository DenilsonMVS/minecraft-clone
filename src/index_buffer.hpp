
#ifndef INDEX_BUFFER_HPP
#define INDEX_BUFFER_HPP

#include <span>

#include "utils.hpp"


template<typename T>
class IndexBuffer {
public:
	static_assert(
		std::is_same<T, unsigned char>::value ||
		std::is_same<T, unsigned short>::value ||
		std::is_same<T, unsigned>::value);


	IndexBuffer() :
		count(0)
	{
		glGenBuffers(1, &this->id);
	}
	
	IndexBuffer(const std::span<const T> &data, const unsigned mode) :
		count(data.size())
	{
		glGenBuffers(1, &this->id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), mode);
	}

	IndexBuffer(IndexBuffer &&other) :
		id(other.id), count(other.count)
	{
		other.id = 0;
	}

	IndexBuffer &operator=(IndexBuffer &&other) {
		if(this != &other) {
			glDeleteBuffers(1, &this->id);
			this->id = other.id;
			other.id = 0;

			this->cout = other.count;
		}
		return *this;
	}

	IndexBuffer(const IndexBuffer &other) = delete;
	IndexBuffer &operator=(const IndexBuffer &other) = delete;

	~IndexBuffer() {
		glDeleteBuffers(1, &this->id);
	}


	unsigned get_id() const {
		return this->id;
	}

	unsigned get_count() const {
		return this->count;
	}

	int get_capacity() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
		int buffer_size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
		return buffer_size / sizeof(T);
	}

	static constexpr unsigned get_type() {
		return det_gl::get_gl_enum<T>();
	}


	void bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
	}

	void reserve(const int minimum_capacity, const unsigned mode = GL_DYNAMIC_DRAW) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);

		int buffer_size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);

		if(minimum_capacity * sizeof(T) > buffer_size)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, minimum_capacity * sizeof(T), nullptr, mode);
	}

	void assign_data(const std::span<T> &data, const unsigned mode = GL_DYNAMIC_DRAW) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);

		int buffer_size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);

		if(data.size() * sizeof(T) > buffer_size)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), mode);
		else
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, data.size() * sizeof(T), data.data());
	}

private:
	unsigned id;
	unsigned count;
};

#endif
