
#ifndef VERTEX_BUFFER_LAYOUT_HPP
#define VERTEX_BUFFER_LAYOUT_HPP

#include <vector>


class VertexBufferLayout {
public:

	struct VertexBufferElement {
		int count;
		unsigned type;
		int normalized;
	};


	VertexBufferLayout();
	void push(const int count, const unsigned type, const int normalized);

	unsigned get_stride() const;
	const std::vector<VertexBufferElement> &get_elements() const;

private:

	std::vector<VertexBufferElement> elements;
	unsigned stride;
};

#endif
