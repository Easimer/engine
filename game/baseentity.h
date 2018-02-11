#pragma once

#include "basethinker.h"
#include "vector.h"
//#include "physobj.h"
//#include "keyvalues.h"

#define DEC_CLASS(classname, basename) \
	typedef basename BaseClass; \
	virtual const char* get_classname() { return #classname; }
#define REGISTER_ENTITY(classname, mapname) static entity_factory<classname> mapname(#mapname);

class base_entity : public base_thinker
{
public:
	virtual void init() = 0;
	~base_entity() {}
	virtual void precache() = 0;
	virtual void spawn() { m_vecPos = vec3_origin; SetNextThink(DONT_THINK); SetParent(NULL); };
	virtual bool is_drawable() { return false; }

	virtual const char* get_classname() = 0;

	virtual vec3 get_pos() { return m_vecPos; }
	virtual void set_pos(vec3 v) { m_vecPos = v; }

	virtual vec3 get_abspos() {
		if (m_pParent)
		{
			// HACKHACKHACK
			m_vecAbsPos = m_pParent->get_abspos() + get_pos();
			return m_vecAbsPos;
		}
		else
		{
			return m_vecPos;
		}
	}

	virtual vec3 GetRelPos() { return m_vecPos; }

	/* TODO: implement these properly */
	virtual void SetAbsPos(vec3 v) {
		return set_pos(v);
	}

	virtual void SetRelPos(vec3 v) {
		return set_pos(v);
	}

	virtual void SetParent(base_entity* pEnt) { 
		if (pEnt == NULL)
		{
			vec3 v = get_abspos();
			m_pParent = NULL;
			SetAbsPos(v);
		}
		else
		{
			m_pParent = pEnt;
			set_pos(vec3_origin);
		}
			
	}
	virtual base_entity* get_parent() { return m_pParent; }

	//virtual CEntityKeyValues* GetEntKV() { return NULL; }

protected:
	// This is the absolute pos if this ent has no parent
	// Otherwise this is relative
	vec3 m_vecPos;
	vec3 m_vecAbsPos;

	base_entity* m_pParent = NULL;
	//CPhysObj* m_pPhysObj = NULL;

private:
	friend class entsys;
};

typedef void(base_entity::*thinkfunc)();

base_entity* CreateEntity(const char* mapname);
base_entity* CreateEntityNoSpawn(const char* mapname);
#define CREATE_ENTITY(mapname) CreateEntity(mapname)
#define CREATE_ENTITY_NOSPAWN(mapname) CreateEntityNoSpawn(mapname)
#define KILL_ENTITY(ent) gpGlobals->pEntSys->kill_entity(ent)

//#include "keyvalues2.h"
