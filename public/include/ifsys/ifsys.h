#pragma once

#include <ifsys/imodule.h>
#include <dl.h>
#include <map>
#include <string>
#include <thread>

// A module implements one or more interfaces. Furthermore a module
// implements a factory function, which takes an ifsys* and a
// module identifier. This identifier contains the module name and
// version.
// 

// Connects modules together
// Also maintains a list of threads that are
// used by these modules
class ifsys {
public:
	virtual void connect(const char* pszName, void* pModule);
	virtual void* query(const char* pszName);
	void shutdown();

	std::shared_ptr<std::thread> make_thread();
private:
	std::map<std::string, void*> m_modules;
	std::vector<std::shared_ptr<std::thread>> m_threads;
};

std::string module_filename(const std::string& name);

typedef void(*IfSysFn)(ifsys*);

#define LINK_MODULE(mod) link_dll<void, ifsys*>(mod, "ifsys_fn")
