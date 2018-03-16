#include "stdafx.h"
#include "framebuffer.h"
#include "glad/glad.h"
#include "assert_opengl.h"

framebuffer::framebuffer()
{
	m_iID = 0;
	glGenFramebuffers(1, &m_iID); ASSERT_OPENGL();
	if (m_iID == 0) {
		PRINT_ERR("framebuffer::framebuffer: Couldn't generate framebuffer!");
		ASSERT(m_iID == 0);
		return;
	}
	PRINT_DBG("framebuffer::framebuffer: generated framebuffer " << m_iID);
}

framebuffer::~framebuffer()
{
	if (m_iID != 0) {
		glDeleteFramebuffers(1, &m_iID); ASSERT_OPENGL();
		m_iID = 0;
	}
}

void framebuffer::bind(framebuffer_bind_target bt)
{
	if (m_iID != 0) {
		switch (bt) {
		case FBBT_DEFAULT:
			glBindFramebuffer(GL_FRAMEBUFFER, m_iID);
			break;
		case FBBT_READ:
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_iID);
			break;
		case FBBT_DRAW:
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_iID);
			break;
		default:
			PRINT_DBG("framebuffer::bind: unknown target " << bt);
			break;
		}
	}
}
