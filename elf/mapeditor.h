#pragma once
#include <ifsys/ifsys.h>
#include <elf/imapeditor.h>
#include <gfx/gfx.h>
#include <gfx/camera.h>
#include <thread>
#include <map>
#include <array>
#include "input.h"

class mapeditor : public imapeditor {
public:
	virtual ~mapeditor() {
		while (m_thread.joinable())
			m_thread.join();
	}
	virtual const char * name() const override {
		return "EngineLevelEditor0001";
	}
	virtual bool shutdown() override;
	virtual void init() override;

	void set_ifsys(ifsys* pIfSys) { m_pIfSys = pIfSys; }

	void handle_events();
	void draw_gui();
	void draw_world();

	void new_world();

	enum tool {
		E_LED_TOOL_MIN = 0,
		E_LED_TOOL_SELECT = 0,
		E_LED_TOOL_TERRAIN_RAISE,
		E_LED_TOOL_TERRAIN_LOWER,
		E_LED_TOOL_MAX
	};

	void load_icon(mapeditor::tool id, const std::string& filename);

	virtual void add_object(const char* szFilename) override;

	struct world_object {
		gfx::model_id iModel;
		bool bStatic;
		vec3 vecPos;
		vec3 vecRot;
		std::string szModel;
	};

	virtual bool is_shutdown() override {
		return m_bShutdown;
	}

private:
	ifsys* m_pIfSys;
	gfx::gfx_global* m_pGfx;
	std::thread m_thread;
	bool m_bShutdown;

	input m_input;
	gfx::camera m_camera;

	bool m_bFreeCamera;
	float m_flGameViewX, m_flGameViewY;
	std::map<mapeditor::tool, gfx::shared_tex2d> m_icons;

	// Editor world state
	std::vector<world_object> m_objects;

	// GUI State
	bool m_bGUILevelSelectorOpen = false;
	std::array<char, 64> m_szGUILevelSelectorMap;
};
