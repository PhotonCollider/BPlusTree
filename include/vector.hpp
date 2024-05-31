/**
 * has an extra sort function, compared to the vector assignment
 */	
#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"
#include "util.hpp" // sort

#include <climits>
#include <cstddef>
#include <iostream> // std::allocator (isnt this weird?)

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector {
private:
	std::allocator<T> myAllocator;
	T* data;
	size_t used, max;
public:
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	 /**
	  * you can see RandomAccessIterator at CppReference for help.
	  */
	class const_iterator;
	class iterator {
		// The following code is written for the C++ type_traits library.
		// Type traits is a C++ feature for describing certain properties of a type.
		// For instance, for an iterator, iterator::value_type is the type that the 
		// iterator points to. 
		// STL algorithms and containers may use these type_traits (e.g. the following 
		// typedef) to work properly. In particular, without the following code, 
		// @code{std::sort(iter, iter1);} would not compile.
		// See these websites for more information:
		// https://en.cppreference.com/w/cpp/header/type_traits
		// About value_type: https://blog.csdn.net/u014299153/article/details/72419713
		// About iterator_category: https://en.cppreference.com/w/cpp/iterator
		friend class vector;

	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		T* ptr;
		vector<T>* const ptr_vec;

		iterator(T* _ptr, vector<T>* _ptr_vec) : ptr_vec(_ptr_vec) {
			ptr = _ptr;
		}

		iterator(const iterator& copy) : ptr_vec(copy.ptr_vec) {
			ptr = copy.ptr;
		}
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 * as well as operator-
		 */
		iterator operator+(const int& n) const {
			return iterator(ptr + n, ptr_vec);
		}
		iterator operator-(const int& n) const {
			return iterator(ptr - n, ptr_vec);
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		int operator-(const iterator& rhs) const {
			if (ptr_vec != rhs.ptr_vec) {
				throw invalid_iterator();
			} else {
				return ptr - rhs.ptr;
			}
		}
		int operator-(const const_iterator& rhs) const {
			if (ptr_vec != rhs.ptr_vec) {
				throw invalid_iterator();
			} else {
				return ptr - rhs.ptr;
			}
		}
		iterator& operator+=(const int& n) {
			ptr += n;
		}
		iterator& operator-=(const int& n) {
			ptr -= n;
		}
		iterator operator++(int) {
			iterator ret(*this);
			++ptr;
			return ret;
		}
		iterator& operator++() {
			++ptr;
			return *this;
		}
		iterator operator--(int) {
			iterator ret(*this);
			--ptr;
			return ret;
		}
		iterator& operator--() {
			--ptr;
			return *this;
		}
		T& operator*() const {
			return *ptr;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory address).
		 */
		bool operator==(const iterator& rhs) const {
			return ptr == rhs.ptr;
		}
		bool operator==(const const_iterator& rhs) const {
			return ptr == rhs.ptr;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator& rhs) const {
			return ptr != rhs.ptr;
		}
		bool operator!=(const const_iterator& rhs) const {
			return ptr != rhs.ptr;
		}
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator {
		friend class vector;

	private:
		const T* ptr;
		const vector<T>* const ptr_vec;

		const_iterator(T* _ptr, const vector<T>* _ptr_vec) : ptr(_ptr), ptr_vec(_ptr_vec) {}

		const_iterator(const iterator& copy) : ptr(copy.ptr), ptr_vec(copy.ptr_vec) {}

	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

		/**
		 * return a new iterator which pointer n-next elements
		 * as well as operator-
		 */
		const_iterator operator+(const int& n) const {
			return const_iterator(ptr + n, ptr_vec);
		}
		const_iterator operator-(const int& n) const {
			return const_iterator(ptr - n, ptr_vec);
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		int operator-(const const_iterator& rhs) const {
			if (ptr_vec != rhs.ptr_vec) {
				throw invalid_iterator();
			} else {
				return ptr - rhs.ptr;
			}
		}
		int operator-(const iterator& rhs) const {
			if (ptr_vec != rhs.ptr_vec) {
				throw invalid_iterator();
			} else {
				return ptr - rhs.ptr;
			}
		}
		const_iterator& operator+=(const int& n) {
			ptr += n;
		}
		const_iterator& operator-=(const int& n) {
			ptr -= n;
		}
		const_iterator operator++(int) {
			const_iterator ret(*this);
			++ptr;
			return ret;
		}
		const_iterator& operator++() {
			++ptr;
			return *this;
		}
		const_iterator operator--(int) {
			const_iterator ret(*this);
			--ptr;
			return ret;
		}
		const_iterator& operator--() {
			--ptr;
			return *this;
		}
		const T& operator*() const {
			return *ptr;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory address).
		 */
		bool operator==(const iterator& rhs) const {
			return ptr == rhs.ptr;
		}
		bool operator==(const const_iterator& rhs) const {
			return ptr == rhs.ptr;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator& rhs) const {
			return ptr != rhs.ptr;
		}
		bool operator!=(const const_iterator& rhs) const {
			return ptr != rhs.ptr;
		}
	};
	/**
	 * TODO Constructs
	 * At least two: default constructor, copy constructor
	 */
	vector() : myAllocator() {
		used = 0, max = 1;
		data = myAllocator.allocate(max);
	}
	vector(const vector& other) : myAllocator() {
		used = other.used, max = other.max;
		data = myAllocator.allocate(max);
		for (size_t i = 0; i < used; i++) {
			new(data + i) T(other.data[i]);
		}
	}
	/**
	 * TODO Destructor
	 */
	~vector() {
		for (size_t i = 0; i < used; i++) {
			data[i].~T();
		}
		myAllocator.deallocate(data, max);
	}
	/**
	 * TODO Assignment operator
	 */
	vector& operator=(const vector& other) { //~destruct then reconstruct
		if (&other == this) {
			return *this;
		}
		// vector::~vector(); is WRONG!!!!
		this->~vector();
		used = other.used, max = other.max;
		data = myAllocator.allocate(max);
		for (size_t i = 0; i < used; i++) {
			new(data + i) T(other.data[i]);
		}
		return *this;
	}

	// the following four lines of code are the same
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T& at(const size_t& pos) {
		if (0 <= pos && pos < used) {
			return *(data + pos);
		} else {
			throw index_out_of_bound();
		}
	}
	const T& at(const size_t& pos) const {
		if (0 <= pos && pos < used) {
			return *(data + pos);
		} else {
			throw index_out_of_bound();
		}
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T& operator[](const size_t& pos) {
		if (0 <= pos && pos < used) {
			return *(data + pos);
		} else {
			throw index_out_of_bound();
		}
	}
	const T& operator[](const size_t& pos) const {
		if (0 <= pos && pos < used) {
			return *(data + pos);
		} else {
			throw index_out_of_bound();
		}
	}
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T& front() const {
		if (used == 0) {
			throw container_is_empty();
		} else {
			return *data;
		}
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T& back() const {
		if (used == 0) {
			throw container_is_empty();
		} else {
			return *(data + used - 1);
		}
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
		return iterator(data, this);
	}
	const_iterator cbegin() const {
		return const_iterator(data, this);
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
		return iterator(data + used, this);
	}
	const_iterator cend() const {
		return const_iterator(data + used, this);
	}
	/**
	 * checks whether the container is empty
	 */
	bool empty() const {
		return used == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
		return used;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		this->~vector();
		used = 0, max = 1;
		data = myAllocator.allocate(max);
	}
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T& value) {
		size_t insert_pos = pos.ptr - data;
		if (used == max) {
			T* new_data = myAllocator.allocate(max * 2);
			for (size_t i = 0; i < insert_pos; i++) {
				new(new_data + i) T(data[i]);
				data[i].~T();
			}
			new(new_data + insert_pos) T(value);
			for (size_t i = insert_pos + 1; i < used + 1; i++) {
				new(new_data + i) T(data[i - 1]);
				data[i - 1].~T();
			}
			myAllocator.deallocate(data, max);

			data = new_data;
			++used;
			max *= 2;
		} else {
			for (size_t i = used; i > insert_pos; i--) {
				data[i] = data[i - 1];
			}
			data[insert_pos] = value;
			++used;
		}
		return pos;
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t& ind, const T& value) {
		if (ind > used) {
			throw index_out_of_bound();
		}
		if (used == max) {
			T* new_data = myAllocator.allocate(max * 2);
			for (size_t i = 0; i < ind; i++) {
				new(new_data + i) T(data[i]);
				data[i].~T();
			}
			myAllocator.construct(new_data + ind, value);
			for (size_t i = ind + 1; i < used + 1; i++) {
				new(new_data + i) T(data[i - 1]);
				data[i - 1].~T();
			}
			myAllocator.deallocate(data, max);

			data = new_data;
			++used;
			max *= 2;
		} else {
			for (size_t i = used; i > ind; i--) {
				data[i] = data[i - 1];
			}
			data[ind] = value;
			++used;
		}
		return iterator(data + ind, this);
	}
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) {
		for (size_t i = pos.ptr - data; i < used - 1; i++) {
			data[i] = data[i + 1];
		}
		data[--used].~T();
		return pos;
	}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t& ind) {
		for (size_t i = ind; i < used - 1; i++) {
			data[i] = data[i + 1];
		}
		data[--used].~T();
		return iterator(data + ind, this);
	}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T& value) {
		if (used == max) {
			T* new_data = myAllocator.allocate(max * 2);
			for (size_t i = 0; i < used; i++) {
				new(new_data + i) T(data[i]);
				data[i].~T();
			}
			myAllocator.deallocate(data, max);
			data = new_data;
			max *= 2;
		}
		new(data + used) T(value);
		++used;
	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() {
		if (used == 0) {
			throw container_is_empty();
		} else {
			data[--used].~T();
		}
	}

	void sort() {
		qsort(data, data + used);
	}
};
}

#endif

