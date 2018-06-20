#pragma once

#include <memory>
#include <gfx/texture.h>

namespace gfx {

class framebuffer {
public:
	framebuffer(size_t nWidth = 0, size_t nHeight = 0);
	~framebuffer();
	framebuffer(framebuffer& f);

	void size(size_t nWidth, size_t nHeight);

	void bind();
	void bindr();
	void bindw();
	void unbind();

	uint32_t name() const { return m_iFramebuffer; }
	gfx::shared_tex2d diffuse() const { return m_iTexDiffuse; }
	gfx::shared_tex2d normal() const { return m_iTexNormal; }
	gfx::shared_tex2d worldpos() const { return m_iTexWorldPos; }
	gfx::shared_tex2d specular() const { return m_iTexSpecular; }
	gfx::shared_tex2d selfillum() const { return m_iTexSelfillum; }
	uint32_t depth() const { return m_iTexDepth; }
private:
	uint32_t m_iFramebuffer;
	gfx::shared_tex2d m_iTexDiffuse;
	gfx::shared_tex2d m_iTexNormal;
	gfx::shared_tex2d m_iTexWorldPos;
	gfx::shared_tex2d m_iTexSpecular;
	gfx::shared_tex2d m_iTexSelfillum;
	uint32_t m_iTexDepth;
	//uint32_t m_iTexDiffuse;
	//uint32_t m_iTexNormal;
	//uint32_t m_iTexDepth;
	size_t m_nWidth, m_nHeight;
};

using shared_fb = std::shared_ptr<framebuffer>;

}