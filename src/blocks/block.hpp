
#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <array>


class Block {
public:
	enum class Id : unsigned char {
		AIR,
		DIRT,
		GRASS,
		STONE,
		WATER,
		NUM_BLOCKS,
		NONE
	};

	bool invisible: 1;
	bool transparent: 1;
	bool solid: 1;

	Block(const bool invisible, const bool transparent, const bool solid);
	virtual ~Block();

	static const Block &get_block(const Id id);

private:
	static const std::array<const Block *, (size_t) Id::NUM_BLOCKS> blocks;
};


#endif
