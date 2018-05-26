#include "stdafx.h"
#include <math/vector.h>

// SIMD specializations for T=float
namespace math {

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

	template<>
	inline float vector3<float>::length_sq() const {
		__m128 l = _mm_load_ps(m_nValues);
		__m128 res = _mm_mul_ps(l, l);
		res = _mm_hadd_ps(res, res);
		res = _mm_hadd_ps(res, res);
		return _mm_cvtss_f32(res);
	}

	template<>
	inline float dot(const vector3<float>& lhs, const vector3<float>& rhs) {
		__m128 l = _mm_load_ps(lhs.ptr());
		__m128 r = _mm_load_ps(rhs.ptr());
		__m128 res = _mm_mul_ps(l, l);
		res = _mm_hadd_ps(res, res);
		res = _mm_hadd_ps(res, res);
		return _mm_cvtss_f32(res);
	}

	template<>
	void vector3<float>::normalize() {
		float len = length();
		__m128 l = _mm_load_ps(m_nValues);
		__m128 r = _mm_set_ps1(len);
		__m128 res = _mm_div_ps(l, r);
		_mm_store_ps(m_nValues, res);
	}

	template<>
	vector3<float> vector3<float>::normalized() const {
		vector3<float> ret;
		float len = length();
		__m128 l = _mm_load_ps(m_nValues);
		__m128 r = _mm_set_ps1(len);
		__m128 res = _mm_div_ps(l, r);
		_mm_store_ps(ret.ptr(), res);
		return ret;
	}
}