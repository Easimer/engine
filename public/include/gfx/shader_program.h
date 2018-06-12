#pragma once

#include <math/vector.h>
#include <gfx/shader.h>
#include <gfx/material.h>
#include <qc.h>
#include <gfx/light.h>
#include <memory>
#include <watchdog.h>

namespace gfx {

	enum shader_tex_type {
		SHADERTEX_DIFFUSE = 0,
		SHADERTEX_NORMAL = 1,
		SHADERTEX_SPECULAR = 2,
		SHADERTEX_OPACITY = 3,
		SHADERTEX_MAX = 4,
		SHADERTEX_DEPTH = 5,
	};

	struct shader_program_light_uniform {
		int flPosX, flPosY, flPosZ;
		int flColorR, flColorG, flColorB, flColorA;
		int bLight;
	};

	class shader_program {
	public:
		shader_program();
		shader_program(const char* szFilename);
		~shader_program();
		bool link();
		void use();
		void validate();

		void attach_shader(gfx::shared_shader pShader);

		void print_err();

		const char* get_name() { return m_szName; }
		const char* get_description() { return m_szDescription; }

		void set_mat_trans(void* pMat);
		void set_mat_view(void* pMat);
		void set_mat_proj(void* pMat);

		uint32_t get_id() const { return m_iID; }

		// 
		bool load_material(material& mat);
		// Set as current material
		void use_material(const material& mat);

		void set_vec3(const std::string& name, const math::vector3<float>& v);
		void set_vec4(const std::string& name, const float* v);
		void set_mat4(const std::string& name, const void* m);

		void set_bool(const std::string& name, bool v);

		void set_local_light(const shader_light& l);
		void set_global_light(const shader_light& l);

		void set_float(const std::string& name, float v);
		void set_int(const std::string& name, int v);

	protected:
		int get_uniform_location(const mdlc::qc& qcp, const std::string& name, int* pLocation);

	private:
		uint32_t m_iID;

		//shader* m_pShaderVert;
		//shader* m_pShaderFrag;
		//shader* m_pShaderGeom;
		std::shared_ptr<shader> m_pShaderVert;
		std::shared_ptr<shader> m_pShaderFrag;
		std::shared_ptr<shader> m_pShaderGeom;

		int m_iUniformMatTrans;
		int m_iUniformMatView;
		int m_iUniformMatProj;
		int m_iUniformTex1;
		int m_iUniformTex2;
		int m_iUniformTex3;
		int m_iUniformTex4;
		int m_iUniformTex5;
		int m_iUniformTime;
		int m_iUniformDepthTexture;

		shader_program_light_uniform m_aiUniformLightLocal;
		shader_program_light_uniform m_aiUniformLightGlobal;

		bool m_bLit;

		char m_szName[64] = { 0 };
		char m_szDescription[128] = { 0 };

		// Key in .mat file mapped to texture type
		std::map<shader_tex_type, std::string> m_mapTexKey;
		// Default value for key in .mat file
		std::map<shader_tex_type, std::string> m_mapTexDefault;
	};

	using shared_shader_program = std::shared_ptr<shader_program>;
}
