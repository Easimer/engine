#pragma once

#include <cstdint>
#include <array>

// Adler-32 implementation
// NOTE: result() returns the value in host
// byte order. It's up to the caller how the
// value is stored.

class adler32 {
public:
	adler32() : A(1), B(0) {}

	template<typename T>
	adler32& apply(const T* const data, size_t N) noexcept {
		const uint8_t* const buf = (uint8_t*)data;
		// Convert element count to byte count
		size_t nLength = N * sizeof(T);
		// Caller feeds us data one byte at a time
		if (nLength == 1) {
			A += buf[0];
			if (A >= base)
				A -= base;
			B += A;
			if (B >= base)
				B -= base;
			return *this;
		}

		if (N == 0)
			return *this;

		size_t i = 0;
		
		while (nLength >= nmax) {
			nLength -= nmax;
			size_t n = 347; // nmax / 16
			do {
				A += buf[i + 0]; B += A;
				A += buf[i + 1]; B += A;
				A += buf[i + 2]; B += A;
				A += buf[i + 3]; B += A;
				A += buf[i + 4]; B += A;
				A += buf[i + 5]; B += A;
				A += buf[i + 6]; B += A;
				A += buf[i + 7]; B += A;
				A += buf[i + 8]; B += A;
				A += buf[i + 9]; B += A;
				A += buf[i + 10]; B += A;
				A += buf[i + 11]; B += A;
				A += buf[i + 12]; B += A;
				A += buf[i + 13]; B += A;
				A += buf[i + 14]; B += A;
				A += buf[i + 15]; B += A;
				i += 16;
			} while (--n);
			A %= base;
			B %= base;
		}

		if (nLength) {
			while (nLength >= 16) {
				nLength -= 16;
				A += buf[i + 0]; B += A;
				A += buf[i + 1]; B += A;
				A += buf[i + 2]; B += A;
				A += buf[i + 3]; B += A;
				A += buf[i + 4]; B += A;
				A += buf[i + 5]; B += A;
				A += buf[i + 6]; B += A;
				A += buf[i + 7]; B += A;
				A += buf[i + 8]; B += A;
				A += buf[i + 9]; B += A;
				A += buf[i + 10]; B += A;
				A += buf[i + 11]; B += A;
				A += buf[i + 12]; B += A;
				A += buf[i + 13]; B += A;
				A += buf[i + 14]; B += A;
				A += buf[i + 15]; B += A;
				i += 16;
			}
			while (nLength--) {
				A += buf[i]; B += A;
				i++;
			}
			A %= base;
			B %= base;
		}
		return *this;
	}

	uint32_t result() const noexcept {
		return (B << 16) | A;
	}

private:
	uint32_t A, B;
	const uint32_t base = 65521;
	const uint32_t nmax = 5552;
	const uint32_t nmaxd16 = 347;
};

class adler64 {
public:
	adler64() : A(1), B(0) {}

	template<typename T>
	adler64& apply(const T* const data, size_t N) noexcept {
		const uint8_t* const buf = (uint8_t*)data;
		// Convert element count to byte count
		size_t nLength = N * sizeof(T);
		// Caller feeds us data one byte at a time
		if (nLength == 1) {
			A += buf[0];
			if (A >= base)
				A -= base;
			B += A;
			if (B >= base)
				B -= base;
			return *this;
		}

		if (N == 0)
			return *this;

		size_t i = 0;

		while (nLength >= nmax) {
			nLength -= nmax;
			size_t n = nmaxd16; // nmax / 16
			do {
				A += buf[i + 0]; B += A;
				A += buf[i + 1]; B += A;
				A += buf[i + 2]; B += A;
				A += buf[i + 3]; B += A;
				A += buf[i + 4]; B += A;
				A += buf[i + 5]; B += A;
				A += buf[i + 6]; B += A;
				A += buf[i + 7]; B += A;
				A += buf[i + 8]; B += A;
				A += buf[i + 9]; B += A;
				A += buf[i + 10]; B += A;
				A += buf[i + 11]; B += A;
				A += buf[i + 12]; B += A;
				A += buf[i + 13]; B += A;
				A += buf[i + 14]; B += A;
				A += buf[i + 15]; B += A;
				i += 16;
			} while (--n);
			A %= base;
			B %= base;
		}

		if (nLength) {
			while (nLength >= 16) {
				nLength -= 16;
				A += buf[i + 0]; B += A;
				A += buf[i + 1]; B += A;
				A += buf[i + 2]; B += A;
				A += buf[i + 3]; B += A;
				A += buf[i + 4]; B += A;
				A += buf[i + 5]; B += A;
				A += buf[i + 6]; B += A;
				A += buf[i + 7]; B += A;
				A += buf[i + 8]; B += A;
				A += buf[i + 9]; B += A;
				A += buf[i + 10]; B += A;
				A += buf[i + 11]; B += A;
				A += buf[i + 12]; B += A;
				A += buf[i + 13]; B += A;
				A += buf[i + 14]; B += A;
				A += buf[i + 15]; B += A;
				i += 16;
			}
			while (nLength--) {
				A += buf[i]; B += A;
				i++;
			}
			A %= base;
			B %= base;
		}
		return *this;
	}

	uint64_t result() const noexcept {
		return (B << 32) | A;
	}

private:
	uint64_t A, B;
	const uint64_t base = 4294967291;
	const uint64_t nmax = 5552;
	const uint64_t nmaxd16 = 347;
};