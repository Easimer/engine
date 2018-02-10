#pragma once

// This really makes the CBaseEntity and CHudElement think :thinking:
class base_thinker
{
public:
	float GetNextThink() { return m_flNextThink; };

	void(base_thinker::*GetThinkFunc())() { return m_pThinkFunc; }

	virtual void Think() {}

protected:
	void(base_thinker::*m_pThinkFunc)();
	float m_flNextThink;
};

#define SetThink(func) (m_pThinkFunc = static_cast<void(base_thinker::*)()>((func)))
#define SetNextThink(time) (m_flNextThink = (time))
#define DONT_THINK -1

