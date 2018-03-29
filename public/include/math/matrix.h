#pragma once

namespace math {
	template<typename T, size_t N>
	class matrix_row {
	public:
	public:
		matrix_row(float* pRow) : m_pRow(pRow) {}
		matrix_row(const matrix_row&) = delete;
		
		void operator=(const matrix_row<T, N>& other) {
			memcpy(m_pRow, other.m_pRow, N * sizeof(T));
		}

		void operator=(const float* pArray) {
			memcpy(m_pRow, pArray, N * sizeof(T));
		}

		float operator[](size_t i) const {
			return m_pRow[i];
		}
		float& operator[](size_t i) {
			return m_pRow[i];
		}
	private:
		float* m_pRow;
	};

	template<typename T, size_t N, size_t M>
	class matrix {
	public:
		matrix() : m_flValues({ 0 }) {}

		matrix_row<T, N> operator[](size_t i) {
			return matrix_row<T, N>(m_flValues[i * N]);
		}

		void operator=(const matrix<T, N, M>& other) {
			memcpy(m_flValues, other.m_flValues, N * M * sizeof(T));
		}
	protected:
		float m_flValues[N * M];
	};

	template<typename T, size_t N, size_t M>
	matrix<T, N, M> operator*(const matrix<T, N, M>& lhs, const matrix<T, N, M>& rhs) {
		matrix<T, N, M> ret;

	}

	typedef matrix<float, 3, 3> mat3f;
	typedef matrix<float, 4, 4> mat4f;
}
