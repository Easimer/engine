#pragma once

#include <ifsys/imodule.h>
#include <dl.h>
#include <map>
#include <string>

// A module implements one or more interfaces. Furthermore a module
// implements a factory function, which takes an ifsys* and a
// module identifier. This identifier contains the module name and
// version.
// 

// Connects modules together
class ifsys {
public:
	virtual void connect(const char* pszName, void* pModule);
	virtual void* query(const char* pszName);
	void shutdown();
private:
	std::map<std::string, void*> m_modules;
};

typedef void(*IfSysFn)(ifsys*);

#define LINK_MODULE(mod) link_dll<void, ifsys*>(mod, "ifsys_fn")
