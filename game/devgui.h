#pragma once

#include <vector>
#include <string>

struct devgui_state {
	/// Debug GUI state
	bool m_bShowRendererDebug = false;
	bool m_bShowStatistics = false;
	bool m_bShowInspector = false;
	bool m_bDrawGlobalLightGizmo = false;
	bool m_bShowPhysics = false;

	// Entity Creator
	bool m_bShowEntityCreate = false;
	char m_szClassname[128] = { 0 };
	char m_szTargetname[128] = { 0 };

	// Entity Inspector
	size_t m_iCurEnt = 0;
	char m_szModelPath[128] = { 0 };

	// Physics
	size_t m_iCurPhysObj = 0;
};
