#pragma once

#include <string>
#include <filesystem>

class watchdog {
public:
	watchdog() : m_bBad(true) {}

	watchdog(const std::string& path) : m_path(path), m_bBad(false) {
		std::error_code ec;
		m_path = path;
		m_last_write_time = std::filesystem::last_write_time(m_path, ec);
		if (ec)
			m_bBad = true;
	}
	bool modified() noexcept {
		if (m_bBad)
			return false;
		auto last_write_time = std::filesystem::last_write_time(m_path);
		bool bRet = (last_write_time != m_last_write_time);
		m_last_write_time = last_write_time;

		return bRet;
	}
private:
	std::filesystem::path m_path;
	std::filesystem::file_time_type m_last_write_time;
	bool m_bBad;
};
