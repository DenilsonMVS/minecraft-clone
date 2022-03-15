
#ifndef SUPER_BUFFER_HPP
#define SUPER_BUFFER_HPP

#include <glad/glad.h>
#include <vector>


/*struct MemoryHolder {
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
};*/

struct LayoutElement {
	unsigned count;
	unsigned type;
	unsigned normalized;
};


class SuperBuffer {
public:

	SuperBuffer() = default;

	SuperBuffer(const std::span<const LayoutElement> &layout)
	{
		glCreateVertexArrays(1, &this->vao);
		glCreateBuffers(1, &this->vbo);
		
		glBindVertexArray(this->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

		unsigned vertex_size = 0;
		for(const auto &element : layout)
			vertex_size += element.count * gl::get_size(element.type);

		size_t offset = 0;
		for(unsigned i = 0; i < layout.size(); i++) {
			const auto &element = layout[i];
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i,
				element.count, element.type, element.normalized,
				vertex_size,
				(const void *) offset);
			offset += element.count * gl::get_size(element.type);
		}
	}

	template<typename T>
	SuperBuffer(
		const std::span<const T> &vertex,
		const gl::Usage vertex_buffer_mode,
		const std::span<const LayoutElement> &layout)
	{
		new(this) SuperBuffer(layout);
		this->assign_data(vertex, vertex_buffer_mode);
	}

	SuperBuffer(SuperBuffer &&other) :
		vbo(other.vbo),
		vao(other.vao)
	{
		other.vbo = 0;
		other.vao = 0;
	}

	SuperBuffer &operator=(SuperBuffer &&other) {
		if(this != &other) {
			this->vbo = other.vbo;
			this->vao = other.vao;
	
			other.vbo = 0;
			other.vao = 0;
		}

		return *this;
	}

	SuperBuffer(const SuperBuffer &other) = delete;
	SuperBuffer &operator=(const SuperBuffer &other) = delete;

	~SuperBuffer() {
		glDeleteVertexArrays(1, &this->vao);
		glDeleteBuffers(1, &this->vbo);
	}


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

	void bind() const {
		glBindVertexArray(this->vao);
	}


private:
	unsigned vbo;
	unsigned vao;
};


#endif
