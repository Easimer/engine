#pragma once

namespace mdlc {
	inline bool is_upper(char c)
	{
		return (c >= 'A' && c <= 'Z');
	}

	inline bool is_lower(char c)
	{
		return (c >= 'a' && c <= 'z');
	}

	inline bool is_letter(char c)
	{
		return is_upper(c) || is_lower(c);
	}

	inline bool is_number(char c)
	{
		return (c >= '0' && c <= '9');
	}

	inline bool is_alphanum(char c)
	{
		return is_letter(c) || is_number(c);
	}

	inline bool is_path(char c)
	{
		return is_alphanum(c) || c == '.' || c == '\\' || c == '/' || c == '-' || c == '_' || c == '+' || c == ' ' || c == ',';
	}

	inline bool is_whitespace(char c)
	{
		return c == ' ' || c == '\r' || c == '\n' || c == '\t';
	}
}
