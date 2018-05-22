#pragma once

#include "base_thinker.h"
#include <math/vector.h>
#include "keyvalues.h"

#define DEC_CLASS(classname, basename) \
	typedef basename BaseClass; \
	virtual const char* get_classname() { return #classname; }

#define REGISTER_ENTITY(classname, mapname) static entity_factory<classname> mapname(#mapname);

/// Entity filters
// Match all entities / Don't filter
#define ENT_FILTER_ALL				(0)
// Match all props
#define ENT_FILTER_PROP				(1 << 0)
// Match all local lights
#define ENT_FILTER_LIGHT_LOCAL		(1 << 1)
// Match the global light
#define ENT_FILTER_LIGHT_GLOBAL		(1 << 2)

class base_entity : public base_thinker
{
public:
	virtual ~base_entity() {}
	/// Precache resources here
	virtual void precache() = 0;

	virtual void spawn();

	/// Should we even bother drawing the entity?
	virtual bool is_drawable() {
		return false;
	}

	/// DO NOT override! Use DEC_CLASS instead!
	virtual const char* get_classname() = 0;

	// Returns with the absolute position of the entity.
	//
	// NOTE: The absolute position of the entity's parent (if it has any)
	// will be accounted for.
	virtual vec3 get_abspos() const;

	// Returns with the relative position of the entity.
	//
	// NOTE: If it has no parent, this is the absolute position too.
	virtual vec3 get_relpos() const;

	// Sets the absolute position
	//
	// NOTE: If the entity has a parent, this has no effect
	virtual void set_abspos(const vec3& v);

	// Sets the relative position
	//
	// NOTE: If the entity has no parent, this sets the absolute position.
	virtual void set_relpos(const vec3& v);

	// Sets the parent of the entity.
	// NOTE: When pEnt is NULL or nullptr, the entity will be detached
	// from the parent.
	virtual void set_parent(base_entity* pEnt);

	// Returns a pointer to the parent
	virtual base_entity* get_parent() {
		return m_pParent;
	}

	// Set relative rotation
	// (which is also the absolute rotation for entities without a parent)
	virtual void set_rotation(const vec3& v);

	// Returns the absolute rotation matrix of the entity
	virtual glm::mat4 get_rotation_matrix() const;

	// Returns the relative rotation of the entity.
	virtual vec3 get_relrot() const {
		return m_vecRot;
	}

	virtual float get_scale() const {
		return 1;
	}

	virtual void set_scale(float flScale) {}

	virtual const char* get_targetname() const {
		return m_szTargetname;
	}

	virtual void set_targetname(const char* szNewname) {
		strncpy(m_szTargetname, szNewname, 128);
	}

	template<keyvalues_type T>
	keyvalue_ptr<T> get_keyvalue(const std::string& name) {
		keyvalue_ptr<T> ptr;
		size_t iOff = m_keyvalues[{name, T}];
		ASSERT(iOff);
		ptr.m_pKv = ((char*)this) + iOff;
		return ptr;
	}

	virtual keyvalues_container& get_keyvalues() {
		return m_keyvalues;
	}

	virtual void load_keyvalues() = 0;

	int get_filter() const { return m_nFilter; }

	virtual inline bool networked() const { return m_bNetworked; }
	virtual inline void networked(bool b) { m_bNetworked = b; }

	inline size_t edict() const { return m_iEdict; }
	inline void edict(size_t i) { m_iEdict = i; }

	virtual bool is_player() const {
		return false;
	}

protected:
	// This is the absolute pos if this ent has no parent
	// Otherwise this is relative
	vec3 m_vecPos;

	// X Y Z Euler
	// Converted to glm::mat4 before generating transform matrix
	vec3 m_vecRot;

	base_entity* m_pParent = NULL;
	//CPhysObj* m_pPhysObj = NULL;

	size_t m_iModelID;

	bool m_bSpawned = false;

	// human-readable name in the IO system
	char m_szTargetname[128] = {0};

	// base keyvalues
	keyvalues_container m_keyvalues = {
		{{"position", KV_T_VECTOR3}, classoffset(base_entity, m_vecPos)},
		{{"rotation", KV_T_VECTOR3 }, classoffset(base_entity, m_vecRot)},
	};

	int m_nFilter = 0;

	bool m_bNetworked = false;
	size_t m_iEdict;

private:
	friend class entsys;
};

typedef void(base_entity::*thinkfunc)();

base_entity* CreateEntity(const char* mapname);
base_entity* CreateEntityNoSpawn(const char* mapname);
#define CREATE_ENTITY(mapname) CreateEntity(mapname)
#define CREATE_ENTITY_NOSPAWN(mapname) CreateEntityNoSpawn(mapname)
#define KILL_ENTITY(ent) gpGlobals->pEntSys->kill_entity(ent)
