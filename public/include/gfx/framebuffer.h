#pragma once

namespace gfx {

class framebuffer {
public:
	framebuffer(size_t nWidth = 0, size_t nHeight = 0);
	~framebuffer();
	framebuffer(framebuffer& f);

	void bind();
	void unbind();

	uint32_t name() const { return m_iFramebuffer; }
	const uint32_t diffuse() const { return m_iTexDiffuse; }
	const uint32_t depth() const { return m_iTexDepth; }

	// Return texture ID and release ownership
	uint32_t release_diffuse() {
		auto ret = m_iTexDiffuse;
		m_iTexDiffuse = 0;
		return ret;
	}

	// Return texture ID and release ownership
	uint32_t release_depth() {
		auto ret = m_iTexDepth;
		m_iTexDepth = 0;
		return ret;
	}
private:
	uint32_t m_iFramebuffer;
	uint32_t m_iTexDiffuse;
	uint32_t m_iTexNormal;
	uint32_t m_iTexDepth;
};

}