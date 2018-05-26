#pragma once

template<class T>
struct assign_lam {
public:
	using callback = std::function<void(T vOld, const T& vNew)>;
	assign_lam(T v, const callback& f) : m_value(v), m_callback(f) {}

	void operator=(const assign_lam& other) {
		m_callback(m_value, other.m_value);
		m_value = other.m_value;
	}
private:
	T m_value;
	callback m_callback;
};
