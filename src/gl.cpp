
#include "gl.hpp"

namespace gl {
	BitField operator|(const BitField f1, const BitField f2) {
		return (BitField) ((unsigned) f1 | (unsigned) f2);
	}
}
