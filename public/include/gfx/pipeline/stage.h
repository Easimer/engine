#pragma once

#include <string>

namespace gfx::pipeline {

enum stage_type {
	stage_intake,
	stage_intermediate,
	stage_delivery
};

enum stage_error {
	stage_err_ok = 0,
	stage_err_bad_shader,
	stage_err_max
};

static const char* stage_error_msg[] = {
	"OK",
	"Bad shader"
};

inline const char* strerror(stage_error c) {
	size_t i = (size_t)c;
	if(i < stage_err_max)
		return stage_error_msg[(size_t)c];
	return "(invalid error code)";
}

class stage {
public:
	stage(bool ready = false) : m_ready(ready) {}
	// Is stage ready
	operator bool() { return m_ready; };

	virtual size_t color() const noexcept { return 0; }
	virtual size_t normal() const noexcept { return 0; }
	virtual size_t worldpos() const noexcept { return 0; }
	virtual size_t specular() const noexcept { return 0; }
	virtual size_t selfillum() const noexcept { return 0; }
	gfx::pipeline::stage_error state() const noexcept { return m_state; }
protected:
	bool m_ready;
	gfx::pipeline::stage_error m_state;
};

struct stage_info {
	std::string name;
	size_t index;
	stage_type type;
	bool ready;
	bool lit;
	stage_error state;

	size_t texture_color;
	size_t texture_normal;
	size_t texture_worldpos;
	size_t texture_specular;
	size_t texture_selfillum;
};

}
