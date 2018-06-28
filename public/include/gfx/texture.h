#pragma once

#include <memory>

namespace gfx {

enum texture_filtering {
	texfilt_nearest,
	texfilt_linear,
	texfilt_nearest_mipmap_nearest,
	texfilt_linear_mipmap_nearest,
	texfilt_nearest_mipmap_linear,
	texfilt_linear_mipmap_linear,
};

enum texture_format {
	texfmt_rgb,
	texfmt_rgba,
	texfmt_depthc,
	texfmt_rgb16f,
	texfmt_rgb32f,
	texfmt_dxt1,
};

enum texture_wrap {
	texw_repeat
};

class texture2d {
public:
	texture2d();
	~texture2d();
	inline uint32_t handle() const {
		return m_hTexture;
	}

	inline operator uint32_t() {
		return m_hTexture;
	}

	void bind(size_t texture_unit = 0);
	void unbind();

	void wrap(texture_wrap wrap);
	void filtering(texture_filtering texfilt);
	void generate_mipmap();

	void upload(const void* pImageData, texture_format iFormat, size_t nWidth, size_t nHeight);
	// Use this to upload compressed textures
	void upload(const void* pImageData, size_t nSize, texture_format iFormat, size_t nWidth, size_t nHeight);

private:
	// Gfx API texture handle
	uint32_t m_hTexture;
	// Has there been a mipmap generated for this texture?
	bool m_bMipmap;
};

using shared_tex2d = std::shared_ptr<gfx::texture2d>;

}
