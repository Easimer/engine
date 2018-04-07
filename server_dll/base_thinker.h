#pragma once

// This really makes the CBaseEntity and CHudElement think :thinking:
class base_thinker
{
public:
	float get_next_think() { return m_next_think; };

	void(base_thinker::*think_func())() { return m_think_func; }

	virtual void think() {}

protected:
	void(base_thinker::*m_think_func)();
	float m_next_think;
};

#define SetThink(func) (m_think_func = static_cast<void(base_thinker::*)()>((func)))
#define SetNextThink(time) (m_next_think = (time))
#define DONT_THINK -1

