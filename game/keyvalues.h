#pragma once

#include <string>
#include <map>
#include "baseentity.h"
#include <gfx/light.h>

#define classoffset(c, member) (size_t)&(((c*)0)->member)

enum keyvalues_type {
	KV_T_VOID		= 0,
	KV_T_VECTOR3	= 1,
	KV_T_RGBA		= 2,
	KV_T_STRING		= 3,
	KV_T_FLOAT		= 4,
	KV_T_INT		= 5,
	KV_T_MAX		= 6
};

typedef std::map<std::pair<std::string, keyvalues_type>, size_t> keyvalues_container;

struct keyvalues_entry {
	std::string iszMapname;
	size_t iOffset;
	keyvalues_type iType;
};

#define BEGIN_KEYVALUES(classname)  typedef classname entkvmaptype; virtual void load_keyvalues() override {

#define END_KEYVALUES() \
	PRINT_DBG("Loaded keyvalues for " << get_classname()); \
	}; \
	virtual keyvalues_container& get_keyvalues() override { \
		return m_keyvalues; \
	}

#define KEYFIELD(name, type, mapname) m_keyvalues[{mapname, type}] = classoffset(entkvmaptype, name)

template<keyvalues_type T>
class keyvalue_ptr : std::false_type {
	void* m_pKv;
};

#define DEF_KV_PTR(T, classname) \
template<> \
class keyvalue_ptr<T> : std::true_type { \
public: \
	classname* operator->() { \
		return (classname*)m_pKv; \
	} \
	void operator=(const classname& val) { \
		(*((classname*)m_pKv)) = val;\
	} \
	classname* ptr() { \
		return (classname*)m_pKv; \
	} \
	void* m_pKv; \
}

DEF_KV_PTR(KV_T_STRING, std::string);
DEF_KV_PTR(KV_T_FLOAT, float);
DEF_KV_PTR(KV_T_RGBA, gfx::color_rgba);
DEF_KV_PTR(KV_T_VECTOR3, vec3);
DEF_KV_PTR(KV_T_INT, int);
