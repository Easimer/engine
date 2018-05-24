#pragma once

#include <array>
#include <mutex>
#include <type_traits>

template<class T, size_t N>
class ring_buffer {
public:
	ring_buffer();

	void push_front(const T& elem);
	// NOTE: Calling this on an empty buffer is undefined
	T pop_back();
	void reset();

	bool empty() { std::lock_guard<std::mutex> lg(m_lock); return m_empty; }
	bool full() { std::lock_guard<std::mutex> lg(m_lock); return m_head == m_tail && !m_empty; }

	static_assert(std::is_copy_constructible<T>::value);
protected:
	void advance_head() {
		m_head++;
		if (m_head == N)
			m_head = 0;
	}

	void advance_tail() {
		m_tail++;
		if (m_tail == N)
			m_tail = 0;
	}
private:
	size_t m_head;
	size_t m_tail;
	bool m_empty;
	std::mutex m_lock;
	std::array<T, N> m_buf;
};

template<class T, size_t N>
ring_buffer<T, N>::ring_buffer() : m_head(0), m_tail(0), m_empty(true) {}

template<class T, size_t N>
void ring_buffer<T, N>::push_front(const T& elem) {
	std::lock_guard<std::mutex> lg(m_lock);
	m_buf[m_head] = elem;
	advance_head();
	if (m_head == m_tail)
		advance_tail();
	m_empty = false;
}

template<class T, size_t N>
T ring_buffer<T, N>::pop_back() {
	std::lock_guard<std::mutex> lg(m_lock);
	T& ret = m_buf[m_tail];

	advance_tail();

	return ret;
}

template<class T, size_t N>
void ring_buffer<T, N>::reset() {
	std::lock_guard<std::mutex> lg(m_lock);
	m_head = m_tail;
	m_empty = true;
}
