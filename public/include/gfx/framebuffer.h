#pragma once

namespace gfx {

class framebuffer {
public:
	framebuffer();
	~framebuffer();
	framebuffer(framebuffer& f);
private:
	uint32_t m_iFramebuffer;
	uint32_t m_iTexDiffuse;
	uint32_t m_iTexNormal;
	uint32_t m_iTexDepth;
};

}