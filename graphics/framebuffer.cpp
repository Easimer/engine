#include "stdafx.h"
#include "glad/glad.h"
#include <gfx/framebuffer.h>
#include "assert_opengl.h"
#include <gfx/shader_program.h>

using namespace gfx;

gfx::framebuffer::framebuffer() {
	PRINT_DBG("[ Framebuffer creation ]");
	
	glGenFramebuffers(1, &m_iFramebuffer);
	PRINT_DBG("Name: " << m_iFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_iFramebuffer);

	glGenTextures(1, &m_iTexDiffuse);
	PRINT_DBG("Diffuse: " << m_iTexDiffuse);
	glBindTexture(GL_TEXTURE_2D, m_iTexDiffuse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gpGfx->width(), gpGfx->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	/*glGenTextures(1, &m_iTexNormal);
	PRINT_DBG("Normal: " << m_iTexNormal);
	glBindTexture(GL_TEXTURE_2D, m_iTexNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gpGfx->width(), gpGfx->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/
	
	glGenRenderbuffers(1, &m_iTexDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_iTexDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gpGfx->width(), gpGfx->height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iTexDepth);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_iTexDiffuse, 0);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

gfx::framebuffer::~framebuffer() {
	if (m_iTexDiffuse)
		glDeleteTextures(1, &m_iTexDiffuse);
	if (m_iTexDepth)
		glDeleteRenderbuffers(1, &m_iTexDepth);
	if (m_iTexNormal)
		glDeleteTextures(1, &m_iTexNormal);
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
