#pragma once

const int FB_SHADOW_MAP_WIDTH = 1024;
const int FB_SHADOW_MAP_HEIGHT = 1024;

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

	bool bind(framebuffer_bind_target bt = FBBT_DEFAULT);
	void unbind();

	void disable_color_buffer();

	void add_depth_texture();

	bool fail() const;

	void bind_texture();

private:
	unsigned m_iID;
	unsigned m_iTextureDepth;

	bool m_bColorBufferDisabled = false;
	bool m_bDepthTextureAdded = false;
};