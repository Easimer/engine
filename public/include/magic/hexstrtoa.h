#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace mgc {
	/*
		Convert hex string to byte array

		NOTE: This function accepts strings with any kind of character
		set, but it assumes that it's ASCII-based.

		@param CharT the string element type
		@param str the string that will be converted
		@param out_buf the output buffer
		@return A pointer to the start of the output if the
		conversion has succeeded, nullptr otherwise.
	*/
	template<class CharT>
	uint8_t* hexstrtoa(const std::basic_string<CharT>& str, uint8_t* out_buf, size_t bufsiz) {
		size_t i = bufsiz - 1;
		for (auto it = str.crbegin(); it != str.crend();) {
			CharT high, low;
			uint8_t byte;
			low = *it; ++it;
			high = *it; ++it;

			high &= 0x7F;
			low &= 0x7F;

			auto convert_char = [](auto c) -> uint8_t {
				if (c >= 0x30 && c <= 0x39) {
					return ((uint8_t)c) - 0x30;
				}
				if (c >= 0x41 && c <= 0x46) {
					return ((uint8_t)c) - 0x37;
				}
				if (c >= 0x61 && c <= 0x66) {
					return ((uint8_t)c) - 0x57;
				}
				return 0;
			};
			// char to number
			high = convert_char(high);
			low = convert_char(low);
			// combine nibbles
			byte = ((high) << 4) | ((low) << 0);
			out_buf[i] = byte;
			if (i == 0) {
				break;
			}
			i--;
		}
		return out_buf + i;
	}
}
