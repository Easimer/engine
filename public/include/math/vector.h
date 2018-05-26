#pragma once

#include <iostream>
#include <cmath>
#include <glm/fwd.hpp>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace math {

	const float EPSILON = 0.000001f;

	template<typename T>
	class vector3 {
	public:
		vector3(T v1 = (T)0, T v2 = (T)0, T v3 = (T)0) : m_nValues{ v1, v2, v3, 0.f } {}

		vector3(const glm::vec<3, T>& other) {
			m_nValues[0] = other[0];
			m_nValues[1] = other[1];
			m_nValues[2] = other[2];
		}

		operator glm::vec<3, T>() const {
			return glm::vec<3, T>(x(), y(), z());
		}

		// Returns nth component of the vector
		inline T operator[](size_t iIndex) const {
			return m_nValues[iIndex];
		}

		// Returns nth component of the vector
		inline T& operator[](size_t iIndex) {
			return m_nValues[iIndex];
		}

		// Returns X component of the vector
		inline T x() const {
			return m_nValues[0];
		}

		// Returns Y component of the vector
		inline T y() const {
			return m_nValues[1];
		}

		// Returns Z component of the vector
		inline T z() const {
			return m_nValues[2];
		}

		// Returns length of the vector
		T length() const {
			return std::sqrt(length_sq());
		}

		// Returns length squared of the vector
		// (won't perform sqrt)
		inline T length_sq() const {
			return x() * x() + y() * y() + z() * z();
		}

		// Returns greater component
		inline T max() const {
			float ret = x();
			if (y() > ret)
				ret = y();
			if (z() > ret)
				ret = z();
			return ret;
		}

		// Returns smallest component
		inline T min() const {
			float ret = x();
			if (y() < ret)
				ret = y();
			if (z() < ret)
				ret = z();
			return ret;
		}

		void operator+=(const vector3<T>& other) {
			m_nValues[0] += other.x();
			m_nValues[1] += other.y();
			m_nValues[2] += other.z();
		}

		void operator-=(const vector3<T>& other) {
			m_nValues[0] -= other.x();
			m_nValues[1] -= other.y();
			m_nValues[2] -= other.z();
		}

		// Normalizes the vector
		void normalize() {
			float len = length();
			m_nValues[0] /= len;
			m_nValues[1] /= len;
			m_nValues[2] /= len;
		}

		// Returns the normalized vector
		vector3<T> normalized() const {
			vector3<T> ret;
			T len = length();
			ret[0] = x() / len;
			ret[1] = y() / len;
			ret[2] = z() / len;
			return ret;
		}

		// Pointer to the underlying structure
		const T* ptr() const {
			return m_nValues;
		}

		T* ptr() {
			return m_nValues;
		}

	protected:
		alignas(16) T m_nValues[4];
	};

	template<typename T>
	vector3<T> operator+(const vector3<T>& lhs, const vector3<T>& rhs) {
		vector3<T> ret;
		ret[0] = lhs[0] + rhs[0];
		ret[1] = lhs[1] + rhs[1];
		ret[2] = lhs[2] + rhs[2];
		return ret;
	}

	template<typename T>
	vector3<T> operator-(const vector3<T>& lhs, const vector3<T>& rhs) {
		vector3<T> ret;
		ret[0] = lhs[0] - rhs[0];
		ret[1] = lhs[1] - rhs[1];
		ret[2] = lhs[2] - rhs[2];
		return ret;
	}

	template<typename T>
	vector3<T> operator*(T lhs, const vector3<T>& rhs) {
		vector3<T> ret;
		ret[0] = lhs * rhs[0];
		ret[1] = lhs * rhs[1];
		ret[2] = lhs * rhs[2];
		return ret;
	}

	template<typename T>
	vector3<T> operator/(const vector3<T>& lhs, T rhs) {
		vector3<T> ret;
		ret[0] = lhs[0] / rhs;
		ret[1] = lhs[1] / rhs;
		ret[2] = lhs[2] / rhs;
		return ret;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const vector3<T>& v) {
		os << '(' << v[0] << ',' << v[1] << ',' << v[2] << ')';
		return os;
	}

	template<typename T>
	vector3<T> max(const vector3<T>& lhs, const vector3<T>& rhs) {
		vector3<T> ret;
		ret[0] = lhs.x() > rhs.x() ? lhs.x() : rhs.x();
		ret[1] = lhs.y() > rhs.y() ? lhs.y() : rhs.y();
		ret[2] = lhs.z() > rhs.z() ? lhs.z() : rhs.z();
		return ret;
	}

	template<typename T>
	vector3<T> min(const vector3<T>& lhs, const vector3<T>& rhs) {
		vector3<T> ret;
		ret.x() = lhs.x() < rhs.x() ? lhs.x() : rhs.x();
		ret.y() = lhs.y() < rhs.y() ? lhs.y() : rhs.y();
		ret.z() = lhs.z() < rhs.z() ? lhs.z() : rhs.z();
		return ret;
	}

	template<typename T>
	inline T dot(const vector3<T>& lhs, const vector3<T>& rhs) {
		return lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z();
	}

	template<typename T>
	inline vector3<T> cross(const vector3<T>& lhs, const vector3<T>& rhs) {
		return vector3<T>(
			lhs[1] * rhs[2] - rhs[1] * lhs[2],
			lhs[2] * rhs[0] - rhs[2] * lhs[0],
			lhs[0] * rhs[1] - rhs[0] * lhs[1]
		);
	}

	template<typename T>
	inline vector3<T> operator-(const vector3<T>& v) {
		return vector3<T>(v[0], v[1], v[2]);
	}

	template<typename T>
	inline bool operator==(const vector3<T>& lhs, const vector3<T>& rhs) {
		if (std::abs(lhs.x() - rhs.x()) > EPSILON) return false;
		if (std::abs(lhs.y() - rhs.y()) > EPSILON) return false;
		if (std::abs(lhs.z() - rhs.z()) > EPSILON) return false;
		return true;
	}

	template<typename T>
	inline bool operator!=(const vector3<T>& lhs, const vector3<T>& rhs) {
		if (std::abs(lhs.x() - rhs.x()) > EPSILON) return true;
		if (std::abs(lhs.y() - rhs.y()) > EPSILON) return true;
		if (std::abs(lhs.z() - rhs.z()) > EPSILON) return true;
		return false;
	}

	template<>
	inline vector3<float> operator+(const vector3<float>& lhs, const vector3<float>& rhs) {
		vector3<float> ret;
		__m128 l = _mm_load_ps(lhs.ptr());
		__m128 r = _mm_load_ps(rhs.ptr());
		__m128 sum = _mm_add_ps(l, r);
		_mm_store_ps(ret.ptr(), sum);
		return ret;
	}

	template<>
	inline vector3<float> operator-(const vector3<float>& lhs, const vector3<float>& rhs) {
		vector3<float> ret;
		__m128 l = _mm_load_ps(lhs.ptr());
		__m128 r = _mm_load_ps(rhs.ptr());
		__m128 diff = _mm_sub_ps(l, r);
		_mm_store_ps(ret.ptr(), diff);
		return ret;
	}

	template<>
	inline vector3<float> operator*(float lhs, const vector3<float>& rhs) {
		vector3<float> ret;
		__m128 l = _mm_set_ps1(lhs);
		__m128 r = _mm_load_ps(rhs.ptr());
		__m128 prod = _mm_mul_ps(l, r);
		_mm_store_ps(ret.ptr(), prod);
		return ret;
	}

	template<>
	inline vector3<float> operator/(const vector3<float>& lhs, float rhs) {
		vector3<float> ret;
		__m128 l = _mm_load_ps(lhs.ptr());
		__m128 r = _mm_set_ps1(rhs);
		__m128 res = _mm_div_ps(l, r);
		_mm_store_ps(ret.ptr(), res);
		return ret;
	}
}

typedef math::vector3<float> vec3;
typedef math::vector3<float> vector;
#define vec3_origin vec3()
