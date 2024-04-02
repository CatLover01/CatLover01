// This is my Custom Implementation of a Vector Container!!!
// Keep in mind that I am still a beginner :3
#include <iostream>
#include <memory>  
#include <initializer_list>
#include <type_traits> // for std::is_trivially_copyable_v
#include <cstring> // for std::memcpy

// This Macro is used when no pointer exist (i.e. when you just cleared the Pector)
#define newCapacity 5

template<typename T>
class Pector {
private:
	int m_size;
	int m_capacity;
	std::unique_ptr<T[]>m_ptr;

public:
	Pector(int capacity) : m_capacity{ list.size() * 2 },
		m_size{ capacity }, m_ptr{ std::make_unique<T[]>(m_capacity) } {}
	Pector(std::initializer_list<T> list) : m_capacity{ list.size() * 2) },
		m_size{ static_cast<int>(list.size()) }, m_ptr{ std::make_unique<T[]>(m_capacity) } {
		int num{ 0 };
		for (auto it : list) {
			m_ptr[num] = *it;
			num++;
		}
	}
	void print() {
		for (int i{}; i < m_size; i++) {
			std::cout << m_ptr[i] << '\n';
		}
	} 
	T& operator[](unsigned int index) {
		if (index < m_size) {
			return m_ptr[index];
		}
		throw std::out_of_range("Access is Out of Bounds");
	}
	void pop_back() {
		if (m_size > 0) {
			m_ptr[m_size - 1] = 0;
			m_size--;
		}
	}
	void push_back(T value) {
		if (!m_ptr) {
			m_ptr = std::make_unique<T[]>(newCapacity); // No magic number so a Macro...
		}
		if (m_size >= m_capacity) {
			m_size++;
			m_capacity = m_size * 2; // Increasing the Array Capacity Geometrically
			auto tempPtr{ std::make_unique<T[]>(m_capacity) };

			if (std::is_trivially_copyable_v<T>) {
				std::memcpy(tempPtr.get(), m_ptr.get(), sizeof m_ptr);
				tempPtr[m_size - 1] = value;
			}
			else {
				for (int i{}; i < m_capacity; i++) {
					if (i == m_capacity - 1) {
						tempPtr[i] = value;
					}
					else {
						tempPtr[i] = m_ptr[i];
					}
				}
			}
			// m_ptr now points to tempPtr
			m_ptr.swap(tempPtr);
			tempPtr.reset();
		}
		else {
			m_size++;
			m_ptr[m_size - 1] = value;
		}
	}
	int size() { return m_size; }
	bool empty() { return m_size <= 0; }
	int capacity() { return m_capacity; }
	void clear() {
		m_size = 0;
		m_capacity = 0;
		m_ptr.reset();
	}
	T* begin() { return m_ptr.get(); } // Iterators
	T* end() { return m_ptr.get() + m_size; }
	const T* begin() const { return m_ptr.get(); }
	const T* end() const { return m_ptr.get() + m_size; }
	const T* cbegin() const { return m_ptr.get(); }
	const T* cend() const { return m_ptr.get() + m_size; }
	std::reverse_iterator<T*> rbegin() { return std::reverse_iterator<T*>(m_ptr.get() + m_size); }
	std::reverse_iterator<T*> rend() { return std::reverse_iterator<T*>(m_ptr.get()); }
	const std::reverse_iterator<T*> rbegin() const { return std::reverse_iterator<T*>(m_ptr.get() + m_size); }
	const std::reverse_iterator<T*> rend() const { return std::reverse_iterator<T*>(m_ptr.get()); }
	const std::reverse_iterator<const T*> crbegin() const { return std::reverse_iterator<const T*>(m_ptr.get() + m_size); }
	const std::reverse_iterator<const T*> crend() const { return std::reverse_iterator<const T*>(m_ptr.get()); }
};
