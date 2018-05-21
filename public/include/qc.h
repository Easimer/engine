#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <parseutils.h>
#include <type_traits>
#include <algorithm>
#include <cstring>

namespace mdlc {
	class qc {
	public:
		qc() {}
		qc(std::istream&& is) {
			for (std::string line; std::getline(is, line);) {
				std::string key, value;
				ptrdiff_t key_start, key_end, value_start, value_end;

				if (line.size() < 3)
					continue;

				if (line[0] == '/' && line[1] == '/')
					continue;

				// Find key
				key_start = std::find(line.cbegin(), line.cend(), '$') - line.cbegin() + 1;
				key_end = std::find_if(line.cbegin() + key_start, line.cend(), mdlc::is_whitespace) - line.cbegin();
				key = line.substr(key_start, key_end - key_start);
				// Find value
				value_start = std::find_if_not(line.cbegin() + key_end, line.cend(), mdlc::is_whitespace) - line.cbegin();
				if (line[value_start] == '\"') {
					value_end = std::find(line.cbegin() + value_start + 1, line.cend(), '\"') - line.cbegin();
					value = line.substr(value_start + 1, value_end - value_start - 1);
				} else {
					value_end = std::find(line.cbegin() + value_start + 1, line.cend(), '\n') - line.cbegin();
					value = line.substr(value_start, value_end - value_start);
				}
				emplace(key, value);
			}
		}

		void save(std::ostream&& os) {
			for (const auto& c : m_map) {
				char buf[256];
				snprintf(buf, 256, "$%s \"%s\"\n", c.first.c_str(), c.second.c_str());
				os.write(buf, strnlen(buf, 256));
			}
		}

		void emplace(const std::string_view& key, const std::string_view& value) {
			m_map.emplace(key, value);
		}

		inline auto begin() noexcept {
			return m_map.begin();
		}

		inline auto end() noexcept {
			return m_map.end();
		}

		inline auto cbegin() const noexcept {
			return m_map.cbegin();
		}

		inline auto cend() const noexcept {
			return m_map.cend();
		}

		inline auto size() const noexcept {
			return m_map.size();
		}

		inline size_t count(const std::string& key) const noexcept {
			return m_map.count(key);
		}

		[[deprecated("Use count")]]
		inline bool is_cmd(const std::string& key) const {
			return count(key);
		}

		template<typename T>
		T at(const std::string& key) const {
			T value;
			if (m_map.count(key)) {
				std::stringstream ss(m_map.at(key));
				if (ss >> value)
					return value;
			}
			return value;
		}

	private:
		std::unordered_map<std::string, std::string> m_map;
	};
}
