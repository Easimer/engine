#include "stdafx.h"
#include "framebuffer.h"
#include "glad/glad.h"
#include "assert_opengl.h"
#include "shader_program.h"

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
		if (m_bDepthTextureAdded) {
			glDeleteTextures(1, &m_iTextureDepth); ASSERT_OPENGL();
		}
	}
}

bool framebuffer::bind(framebuffer_bind_target bt)
{
	if (m_iID != 0) {
		switch (bt) {
		case FBBT_DEFAULT:
			glBindFramebuffer(GL_FRAMEBUFFER, m_iID); ASSERT_OPENGL();
			break;
		case FBBT_READ:
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_iID); ASSERT_OPENGL();
			break;
		case FBBT_DRAW:
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_iID); ASSERT_OPENGL();
			break;
		default:
			PRINT_DBG("framebuffer::bind: unknown target " << bt);
			return false;
			break;
		}
		return true;
	}
	return false;
}

void framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer::disable_color_buffer()
{
	if (!m_bColorBufferDisabled && bind()) {
		glDrawBuffer(GL_NONE); ASSERT_OPENGL();
		glReadBuffer(GL_NONE); ASSERT_OPENGL();
		unbind();
		m_bColorBufferDisabled = true;
	}
}

void framebuffer::add_depth_texture()
{
	if (!m_bDepthTextureAdded && bind()) {
		glGenTextures(1, &m_iTextureDepth); ASSERT_OPENGL();
		glBindTexture(GL_TEXTURE_2D, m_iTextureDepth); ASSERT_OPENGL();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			FB_SHADOW_MAP_WIDTH, FB_SHADOW_MAP_HEIGHT,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);  ASSERT_OPENGL();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); ASSERT_OPENGL();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); ASSERT_OPENGL();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); ASSERT_OPENGL();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); ASSERT_OPENGL();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iTextureDepth, 0);
		unbind();
		m_bDepthTextureAdded = true;
	}
}

bool framebuffer::fail() const
{
	if (m_iID == 0) {
#ifdef PLAT_DEBUG
		PRINT_DBG("framebuffer::fail");
		PRINT_DBG("Framebuffer with ID=" << m_iID << " is not complete:");
		PRINT_DBG("Invalid ID");
#endif
		return true;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, m_iID); ASSERT_OPENGL();
	GLenum iStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);  ASSERT_OPENGL();
	if (iStatus != GL_FRAMEBUFFER_COMPLETE) {
#ifdef PLAT_DEBUG
		PRINT_DBG("framebuffer::fail");
		PRINT_DBG("Framebuffer with ID=" << m_iID << " is not complete:");
		switch (iStatus) {
		case GL_FRAMEBUFFER_UNDEFINED:
			PRINT_DBG("The specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			PRINT_DBG("One or more of the framebuffer attachment points are framebuffer incomplete.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			PRINT_DBG("The framebuffer does not have at least one image attached to it..");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			PRINT_DBG("The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			PRINT_DBG("GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER.");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			PRINT_DBG("The combination of internal formats of the attached images violates an implementation-dependent set of restrictions.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			PRINT_DBG("One or more of the following: ");
			PRINT_DBG("- The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers;");
			PRINT_DBG("- The value of GL_TEXTURE_SAMPLES is the not same for all attached textures;");
			PRINT_DBG("- The attached images are a mix of renderbuffers and textures and the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES;");
			PRINT_DBG("- The value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures, or");
			PRINT_DBG("- The attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			PRINT_DBG("The specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist.");
			break;
		default:
			PRINT_DBG("Unknown error!");
			break;
		}
#endif
		return true;
	}
	return false;
}

void framebuffer::bind_texture()
{
	if (m_iTextureDepth) {
		glActiveTexture(GL_TEXTURE0 + SHADERTEX_DEPTH); ASSERT_OPENGL();
		glBindTexture(GL_TEXTURE_2D, m_iTextureDepth); ASSERT_OPENGL();
	}
}
