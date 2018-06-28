#include "stdafx.h"
#include "glad/glad.h"
#include <gfx/texture.h>

using namespace gfx;

gfx::texture2d::texture2d() {
	glGenTextures(1, &m_hTexture);
	//PRINT_DBG("gfx: created texture " << m_hTexture);
	m_bMipmap = false;
}

gfx::texture2d::~texture2d() {
	if (m_hTexture) {
		if (!gpGfx->api_alive()) {
			// Don't try to call glDeleteTextures if the GL context doesn't exist anymore
			return;
		}
		glDeleteTextures(1, &m_hTexture);
		//PRINT_DBG("gfx: deleted texture " << m_hTexture);
	}
}

void gfx::texture2d::bind(size_t texture_unit) {
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_2D, m_hTexture);
}

void gfx::texture2d::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void gfx::texture2d::wrap(texture_wrap wrap) {
	GLuint val = 0;
	switch (wrap) {
	case texw_repeat:
		val = GL_REPEAT;
		break;
	}
	if (val) {
		bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, val);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, val);
	}
}

void gfx::texture2d::filtering(texture_filtering texf) {
	bind();
	GLuint val_min = 0;
	GLuint val_mag = 0;
	switch (texf) {
	case texfilt_nearest:
		val_min = GL_NEAREST;
		val_mag = GL_NEAREST;
		break;
	case texfilt_linear:
		val_min = GL_LINEAR;
		val_mag = GL_LINEAR;
		break;
	case texfilt_nearest_mipmap_nearest:
		val_min = GL_NEAREST_MIPMAP_NEAREST;
		generate_mipmap();
		val_mag = GL_NEAREST;
		break;
	case texfilt_linear_mipmap_nearest:
		val_min = GL_LINEAR_MIPMAP_NEAREST;
		generate_mipmap();
		val_mag = GL_NEAREST;
		break;
	case texfilt_nearest_mipmap_linear:
		val_min = GL_NEAREST_MIPMAP_LINEAR;
		generate_mipmap();
		val_mag = GL_LINEAR;
		break;
	case texfilt_linear_mipmap_linear:
		val_min = GL_LINEAR_MIPMAP_LINEAR;
		generate_mipmap();
		val_mag = GL_LINEAR;
		break;
	}
	if (val_min || val_mag) {
		bind();
		if (val_min)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, val_min);
		if (val_mag)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, val_mag);
	}
}

void gfx::texture2d::generate_mipmap() {
	if (m_bMipmap)
		return;
	bind();
	glGenerateMipmap(GL_TEXTURE_2D);
	m_bMipmap = true;
}

void gfx::texture2d::upload(const void * pImageData, texture_format iFormat, size_t nWidth, size_t nHeight) {
	GLuint iFmt = 0;
	GLuint iIFmt = 0;
	GLenum iType = 0;
	GLsizei nSize = 0;
	switch (iFormat) {
	case texfmt_rgb:
		iFmt = GL_RGB;
		iIFmt = GL_RGB;
		iType = GL_UNSIGNED_BYTE;
		break;
	case texfmt_rgba:
		iFmt = GL_RGBA;
		iIFmt = GL_RGBA;
		iType = GL_UNSIGNED_BYTE;
		break;
	case texfmt_depthc:
		iFmt = GL_DEPTH_COMPONENT;
		iType = GL_UNSIGNED_SHORT;
		break;
	case texfmt_rgb16f:
		iFmt = GL_RGB16F;
		iIFmt = GL_RGBA;
		iType = GL_HALF_FLOAT;
		break;
	case texfmt_rgb32f:
		iFmt = GL_RGB32F;
		iIFmt = GL_RGBA;
		iType = GL_FLOAT;
		break;
	case texfmt_dxt1:

		break;
	}
	ASSERT(iFmt && iIFmt && iType);
	if (iFmt && iIFmt && iType) {
		bind();
		if (iFormat == texfmt_dxt1) {
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, nWidth, nHeight, 0, 0, pImageData);
		} else {
			glTexImage2D(GL_TEXTURE_2D, 0, iFmt, nWidth, nHeight, 0, iIFmt, iType, pImageData);
		}
	}
}

void gfx::texture2d::upload(const void * pImageData, size_t nSize, texture_format iFormat, size_t nWidth, size_t nHeight) {
	ASSERT(pImageData && nSize && nWidth && nHeight);
	bind();
	if (iFormat == texfmt_dxt1) {
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, nWidth, nHeight, 0, nSize, pImageData);
	} else {
		ASSERT(0);
	}
}
