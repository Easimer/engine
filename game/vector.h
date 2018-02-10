#pragma once

#define vec_x m_aflValues[0]
#define vec_y m_aflValues[1]
#define vec_z m_aflValues[2]

class vector {
public:
	vector() {
	}

	vector(float x) {
		m_aflValues[0] = x;
	}

	vector(float x, float y) {
		m_aflValues[0] = x;
		m_aflValues[1] = y;
	}

	vector(float x, float y, float z) {
		m_aflValues[0] = x;
		m_aflValues[1] = y;
		m_aflValues[2] = z;
	}

	float& operator[](size_t iIndex) {
		return m_aflValues[iIndex];
	}

	float length() const {
		return sqrt(length_sq());
	}

	float length_sq() const {
		return vec_x * vec_x + vec_y * vec_y + vec_z * vec_z;
	}

	float size() const { return 2; }

	void normalize() {
		float len = length();
		for (size_t i = 0; i < 3; i++)
			m_aflValues[i] = m_aflValues[i] / len;
	}

	vector normalized() const {
		float len = length();
		vector ret(m_aflValues[0] / len, m_aflValues[1] / len, m_aflValues[2] / len);
		return ret;
	}

	bool operator==(const vector& other) const {
		for (size_t i = 0; i < 3; i++) {
			if (other.m_aflValues[i] != m_aflValues[i])
				return false;
		}
		return true;
	}

	vector operator+(const vector& other) const {
		vector ret(vec_x + other.vec_x, vec_y + other.vec_y, vec_z + other.vec_z);
		return ret;
	}

	vector operator-(const vector& other) const {
		vector ret(vec_x - other.vec_x, vec_y - other.vec_y, vec_z - other.vec_z);
		return ret;
	}

	vector operator*(const float other) const {
		vector ret(vec_x * other, vec_y * other, vec_z * other);
		return ret;
	}

	vector operator/(const float other) const {
		vector ret(vec_x / other, vec_y / other, vec_z / other);
		return ret;
	}

private:
	float m_aflValues[3] = { 0 };
};

typedef vector vec;
typedef vector vec3;

#define vec3_origin vec3(0,0,0)