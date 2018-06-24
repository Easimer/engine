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
	//PRINT_DBG("Generated framebuffer " << m_iFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_iFramebuffer); ASSERT_OPENGL();

	m_iTexDiffuse = std::make_shared<gfx::texture2d>();
	m_iTexDiffuse->bind();
	m_iTexDiffuse->upload(nullptr, texfmt_rgb, m_nWidth, m_nHeight);
	m_iTexDiffuse->filtering(texfilt_nearest);

	m_iTexNormal = std::make_shared<gfx::texture2d>();
	m_iTexNormal->bind();
	m_iTexNormal->upload(nullptr, texfmt_rgb32f, m_nWidth, m_nHeight);
	m_iTexNormal->filtering(texfilt_nearest);

	m_iTexWorldPos = std::make_shared<gfx::texture2d>();
	m_iTexWorldPos->bind();
	m_iTexWorldPos->upload(nullptr, texfmt_rgb32f, m_nWidth, m_nHeight);
	m_iTexWorldPos->filtering(texfilt_nearest);

	m_iTexSpecular = std::make_shared<gfx::texture2d>();
	m_iTexSpecular->bind();
	m_iTexSpecular->upload(nullptr, texfmt_rgb, m_nWidth, m_nHeight);
	m_iTexSpecular->filtering(texfilt_nearest);

	m_iTexSelfillum = std::make_shared<gfx::texture2d>();
	m_iTexSelfillum->bind();
	m_iTexSelfillum->upload(nullptr, texfmt_rgb, m_nWidth, m_nHeight);
	m_iTexSelfillum->filtering(texfilt_nearest);

	glGenRenderbuffers(1, &m_iTexDepth); ASSERT_OPENGL();
	glBindRenderbuffer(GL_RENDERBUFFER, m_iTexDepth); ASSERT_OPENGL();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_nWidth, m_nHeight); ASSERT_OPENGL();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iTexDepth); ASSERT_OPENGL();
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_iTexDiffuse->handle(), 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_iTexNormal->handle(), 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_iTexWorldPos->handle(), 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, m_iTexSpecular->handle(), 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, m_iTexSelfillum->handle(), 0);
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, DrawBuffers); ASSERT_OPENGL();

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
	if (m_iFramebuffer) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_iFramebuffer);
		//PRINT_DBG("Bound framebuffer (draw): " << m_iFramebuffer);
		//PRINT_DBG("Bound framebuffer (read): " << m_iFramebuffer);
	}
}

void gfx::framebuffer::bindr() {
	if (m_iFramebuffer) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_iFramebuffer);
		//PRINT_DBG("Bound framebuffer (read): " << m_iFramebuffer);
	}
}

void gfx::framebuffer::bindw() {
	if (m_iFramebuffer) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_iFramebuffer);
		//PRINT_DBG("Bound framebuffer (draw): " << m_iFramebuffer);
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, DrawBuffers);
	}
}

void gfx::framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//PRINT_DBG("Bound framebuffer (draw): 0");
	//PRINT_DBG("Bound framebuffer (read): 0");
}

void gfx::framebuffer::size(size_t nWidth, size_t nHeight) {
	m_iTexDiffuse->upload(nullptr, texfmt_rgb, m_nWidth, m_nHeight);
	m_iTexNormal->upload(nullptr, texfmt_rgb, m_nWidth, m_nHeight);
	m_iTexWorldPos->upload(nullptr, texfmt_rgb, m_nWidth, m_nHeight);
	m_iTexSpecular->upload(nullptr, texfmt_rgb, m_nWidth, m_nHeight);
	m_iTexSelfillum->upload(nullptr, texfmt_rgb, m_nWidth, m_nHeight);
}
