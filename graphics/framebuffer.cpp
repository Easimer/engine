#include "stdafx.h"
#include "glad/glad.h"
#include <gfx/framebuffer.h>
#include "assert_opengl.h"
#include <gfx/shader_program.h>

using namespace gfx;

gfx::framebuffer::framebuffer(size_t nWidth, size_t nHeight) {	
	if (nWidth == 0)
		nWidth = gpGfx->width();
	if (nHeight == 0)
		nHeight = gpGfx->height();
	
	glGenFramebuffers(1, &m_iFramebuffer); ASSERT_OPENGL();
	glBindFramebuffer(GL_FRAMEBUFFER, m_iFramebuffer); ASSERT_OPENGL();

	glGenTextures(1, &m_iTexDiffuse); ASSERT_OPENGL();
	glBindTexture(GL_TEXTURE_2D, m_iTexDiffuse); ASSERT_OPENGL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0); ASSERT_OPENGL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); ASSERT_OPENGL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); ASSERT_OPENGL();

	/*glGenTextures(1, &m_iTexNormal);
	PRINT_DBG("Normal: " << m_iTexNormal);
	glBindTexture(GL_TEXTURE_2D, m_iTexNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gpGfx->width(), gpGfx->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/
	
	glGenRenderbuffers(1, &m_iTexDepth); ASSERT_OPENGL();
	glBindRenderbuffer(GL_RENDERBUFFER, m_iTexDepth); ASSERT_OPENGL();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, nWidth, nHeight); ASSERT_OPENGL();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iTexDepth); ASSERT_OPENGL();
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_iTexDiffuse, 0); ASSERT_OPENGL();
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); ASSERT_OPENGL();

	ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

gfx::framebuffer::~framebuffer() {
	if (m_iTexDiffuse) {
		PRINT_DBG("framebuffer diffuse " << m_iTexDiffuse << " DELETED!");
		glDeleteTextures(1, &m_iTexDiffuse);
	}
	if (m_iTexDepth) {
		glDeleteRenderbuffers(1, &m_iTexDepth);
	}
	//if (m_iTexNormal) {
	//	glDeleteTextures(1, &m_iTexNormal);
	//}
	if (m_iFramebuffer)
		glDeleteFramebuffers(1, &m_iFramebuffer);
}

gfx::framebuffer::framebuffer(framebuffer& f) {
	m_iTexDiffuse = f.m_iTexDiffuse;
	m_iTexNormal = f.m_iTexNormal;
	m_iTexDepth = f.m_iTexDepth;
	m_iFramebuffer = f.m_iFramebuffer;
	f.m_iFramebuffer = f.m_iTexDiffuse = f.m_iTexNormal = f.m_iTexDepth = 0;
}

void gfx::framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_iFramebuffer); ASSERT_OPENGL();
}

void gfx::framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0); ASSERT_OPENGL();
}
