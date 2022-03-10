
#ifndef ARR_MAP_KEY
#define ARR_MAP_KEY

#include <array>

namespace det {
	template<typename T, size_t SIZE>
	class arr_map_key : public std::array<T, SIZE> {
		using std::array<T, SIZE>::std::array<T, SIZE>;

		bool operator<(const arr_map_key<T, SIZE> &other) const {
			for(size_t i = 0; i < this->size(); i++)
				if((*this)[i] != other[i])
					return (*this)[i] < other[i];
			return false;
		}
	};
}


#endif
