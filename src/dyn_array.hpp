

#ifndef DYN_ARRAY_HPP
#define DYN_ARRAY_HPP

#include <memory>
#include <cassert>
#include <cstdlib>


namespace det {

	constexpr size_t dyn_extent = -1;


	template<typename T, size_t SIZE>
	class dyn_array {
	public:
		
		template<typename ... ARGS>
		dyn_array(const ARGS & ... args) :
			m_ptr((T *) malloc(sizeof(T) * SIZE))
		{
			for(size_t i = 0; i < SIZE; i++)
				new(&this->m_ptr[i]) T(args ...);
		}

		~dyn_array() {
			this->deleter();
		}

		dyn_array(const dyn_array<T, SIZE> &other) :
			m_ptr((T *) malloc(sizeof(T) * SIZE))
		{
			for(size_t i = 0; i < SIZE; i++)
				new(&this->m_ptr[i]) T(other[i]);
		}
		dyn_array<T, SIZE> &operator=(const dyn_array<T, SIZE> &other) {
			if(&other != this) {
				this->deleter();
				new(this) dyn_array<T, SIZE>(other);
			}

			return *this;
		}

		dyn_array(dyn_array<T, SIZE> &&other) :
			m_ptr(other.m_ptr)
		{
			other.cleaner();
		}
		dyn_array<T, SIZE> &operator=(dyn_array<T, SIZE> &&other) {
			if(&other != this) {
				this->deleter();
				new(this) dyn_array<T, SIZE>(other);
			}

			return *this;
		}


		static constexpr size_t size() {
			return SIZE;
		}


		T *data() {
			return this->m_ptr;
		}

		const T *data() const {
			return this->m_ptr;
		}


		T &operator[](const size_t index) {
			assert(index < SIZE);
			return this->m_ptr[index];
		}

		const T &operator[](const size_t index) const {
			assert(index < SIZE);
			return this->m_ptr[index];
		}

	
		T *begin() {
			return this->m_ptr;
		}

		const T *begin() const{
			return this->m_ptr;
		}

		const T *cbegin() const{
			return this->m_ptr;
		}

		T *end() {
			return &this->m_ptr[SIZE];
		}

		const T *end() const {
			return &this->m_ptr[SIZE];
		}

		const T *cend() const {
			return &this->m_ptr[SIZE];
		}


		T &front() {
			assert(SIZE >= 1);
			return this->m_ptr[0];
		}

		const T &front() const {
			assert(SIZE >= 1);
			return this->m_ptr[0];
		}

		
		T &back() {
			assert(SIZE >= 1);
			return this->m_ptr[SIZE - 1];
		}

		const T &back() const {
			assert(SIZE >= 1);
			return this->m_ptr[SIZE - 1];
		}
	
	private:

		void deleter() {
			if(this->m_ptr != nullptr) {
				for(size_t i = 0; i < SIZE; i++)
					this->m_ptr[i].~T();
				free(this->m_ptr);
			}
		}

		void cleaner() {
			this->m_ptr = nullptr;
		}

		
		T *m_ptr;
	};



	template<typename T>
	class dyn_array<T, dyn_extent> {
	public:

		dyn_array() :
			m_ptr(nullptr), m_size(0) {}
		
		template<typename ... ARGS>
		dyn_array(const size_t size, const ARGS & ... args) :
			m_ptr((T *) malloc(sizeof(T) * size)), m_size(size)
		{
			for(size_t i = 0; i < size; i++)
				new(&this->m_ptr[i]) T(args ...);
		}

		~dyn_array() {
			this->deleter();
		}

		template<typename OTHER_SIZE>
		dyn_array(const dyn_array<T, OTHER_SIZE> &other) :
			m_ptr((T *) malloc(sizeof(T) * other.size())), m_size(other.size())
		{
			for(size_t i = 0; i < this->m_size; i++)
				new(&this->m_ptr[i]) T(other[i]);
		}

		template<typename OTHER_SIZE>
		dyn_array<T, dyn_extent> &operator=(const dyn_array<T, OTHER_SIZE> &other) {
			if(&other != this) {
				this->deleter();
				new(this) dyn_array<T, dyn_extent>(other);
			}

			return *this;
		}

		
		dyn_array(dyn_array<T, dyn_extent> &&other) :
			m_ptr(other.m_ptr), m_size(other.m_size)
		{
			other.cleaner();
		}
		dyn_array<T, dyn_extent> &operator=(dyn_array<T, dyn_extent> &&other) {
			if(&other != this) {
				this->deleter();
				new(this) dyn_array<T, dyn_extent>(other);
			}
			return *this;
		}


		template<typename ... ARGS>
		void resize(const size_t size, const ARGS & ... args) {
			this->m_ptr = realloc(this->m_ptr, sizeof(T) * size);
			for(size_t i = this->m_size; i < size; i++)
				new (&this->m_ptr[i]) T(args ...);
			this->m_size = size;
		}

		void shrink(const size_t size) {
			assert(size <= this->m_size);
			this->m_ptr = realloc(this->m_ptr, sizeof(T) * size);
			this->m_size = size;
		}


		size_t size() const {
			return this->m_size;
		}


		T *data() {
			return this->m_ptr;
		}

		const T *data() const {
			return this->m_ptr;
		}


		T &operator[](const size_t index) {
			assert(index < this->m_size);
			return this->m_ptr[index];
		}

		const T &operator[](const size_t index) const {
			assert(index < this->m_size);
			return this->m_ptr[index];
		}


		T *begin() {
			return this->m_ptr;
		}

		const T *begin() const{
			return this->m_ptr;
		}

		const T *cbegin() const{
			return this->m_ptr;
		}

		T *end() {
			return &this->m_ptr[this->m_size];
		}

		const T *end() const {
			return &this->m_ptr[this->m_size];
		}

		const T *cend() const {
			return &this->m_ptr[this->m_size];
		}


		bool empty() const {
			return this->m_size == 0;
		}


		T &front() {
			assert(this->m_size >= 1);
			return this->m_ptr[0];
		}

		const T &front() const {
			assert(this->m_size >= 1);
			return this->m_ptr[0];
		}

		
		T &back() {
			assert(this->m_size >= 1);
			return this->m_ptr[this->m_size - 1];
		}

		const T &back() const {
			assert(this->m_size >= 1);
			return this->m_ptr[this->m_size - 1];
		}

	
	private:

		void deleter() {
			if(this->m_ptr != nullptr) {
				for(size_t i = 0; i < this->m_size; i++)
					this->m_ptr[i].~T();
				free(this->m_ptr);
			}
		}

		void cleaner() {
			this->m_ptr = nullptr;
			this->m_size = 0;
		}


		T *m_ptr;
		size_t m_size;
	};
}

#endif
