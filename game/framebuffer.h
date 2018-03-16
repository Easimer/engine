#pragma once

enum framebuffer_bind_target {
	FBBT_DEFAULT = 0,
	FBBT_READ = 1,
	FBBT_DRAW = 2,
	FBBT_MAX = 3
};

class framebuffer {
public:
	framebuffer();
	~framebuffer();

	framebuffer(const framebuffer&) = delete;
	void operator=(const framebuffer&) = delete;

	void bind(framebuffer_bind_target bt = FBBT_DEFAULT);

private:
	unsigned m_iID;
};