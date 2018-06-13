#include "stdafx.h"
#include "glad/glad.h"
#include <gfx/framebuffer.h>
#include "assert_opengl.h"
#include <gfx/shader_program.h>

using namespace gfx;

gfx::framebuffer::framebuffer(size_t nWidth, size_t nHeight) {
	if (nWidth == 0)
		m_nWidth = gpGfx->width();
	else
		m_nWidth = nWidth;
	if (nHeight == 0)
		m_nHeight = gpGfx->height();
	else
		m_nHeight = nHeight;

	glGenFramebuffers(1, &m_iFramebuffer); ASSERT_OPENGL();
	PRINT_DBG("Generated framebuffer " << m_iFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_iFramebuffer); ASSERT_OPENGL();

	m_iTexDiffuse = std::make_shared<gfx::texture2d>();
	m_iTexDiffuse->bind();
	m_iTexDiffuse->upload(nullptr, texfmt_rgb, m_nWidth, m_nHeight);
	m_iTexDiffuse->filtering(texfilt_nearest);

	m_iTexNormal = std::make_shared<gfx::texture2d>();
	m_iTexNormal->bind();
	m_iTexNormal->upload(nullptr, texfmt_rgb, m_nWidth, m_nHeight);
	m_iTexNormal->filtering(texfilt_nearest);

	glGenRenderbuffers(1, &m_iTexDepth); ASSERT_OPENGL();
	glBindRenderbuffer(GL_RENDERBUFFER, m_iTexDepth); ASSERT_OPENGL();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_nWidth, m_nHeight); ASSERT_OPENGL();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iTexDepth); ASSERT_OPENGL();
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_iTexDiffuse->handle(), 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_iTexNormal->handle(), 0);
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, DrawBuffers); ASSERT_OPENGL();

	ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

gfx::framebuffer::~framebuffer() {
	if (m_iFramebuffer) {
		glDeleteFramebuffers(1, &m_iFramebuffer);
	}
}

gfx::framebuffer::framebuffer(framebuffer& f) {
	/*m_iTexDiffuse = f.m_iTexDiffuse;
	m_iTexNormal = f.m_iTexNormal;
	m_iTexDepth = f.m_iTexDepth;
	m_iFramebuffer = f.m_iFramebuffer;
	f.m_iFramebuffer = f.m_iTexDiffuse = f.m_iTexNormal = f.m_iTexDepth = 0;*/
	f.m_iFramebuffer = 0;
}

void gfx::framebuffer::bind() {
	if (m_iFramebuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, m_iFramebuffer);
}

void gfx::framebuffer::bindr() {
	if (m_iFramebuffer)
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_iFramebuffer);
}

void gfx::framebuffer::bindw() {
	if(m_iFramebuffer)
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_iFramebuffer);
}

void gfx::framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void gfx::framebuffer::size(size_t nWidth, size_t nHeight) {
	m_iTexDiffuse->upload(nullptr, texfmt_rgb, m_nWidth, m_nHeight);
	m_iTexNormal->upload(nullptr, texfmt_rgb, m_nWidth, m_nHeight);
}
