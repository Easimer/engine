#pragma once

#include <enl/platform.h>
#include <enl/assert.h>
#include <type_traits>

template<class R, class... Args>
class link_dll {
public:
	link_dll(const char* pszModule, const char* pszSymbol)
		: m_pFunc(nullptr), m_pDll(nullptr) {
		ASSERT(pszModule); ASSERT(pszSymbol);
		if (!(pszModule && pszSymbol)) return;
		load_module(pszModule);
		link(pszSymbol);
	}

	~link_dll() { unload_module(); }

	inline operator bool() const {
		return m_pDll && m_pFunc;
	}

	inline R operator()(Args... args) {
		return m_pFunc(std::forward<Args>(args)...);
	}

protected:
	inline void load_module(const char* pszModule) {
#if defined(PLAT_WINDOWS)
		m_pDll = LoadLibraryA(pszModule);
#elif defined(PLAT_LINUX) || defined(PLAT_OSX)
		m_pDll = dlopen(pszModule, RTLD_NOW);
		ASSERT_CUSTOM(m_pDll, dlerror());
#endif
	}

	inline void unload_module() {
		if (!m_pDll) return;
#if defined(PLAT_WINDOWS)
		FreeLibrary(m_pDll);
#elif defined(PLAT_LINUX) || defined(PLAT_OSX)
		dlclose(m_pDll);
#endif
	}

	inline void link(const char* pszSymbol) {
		if (!m_pDll) return;
		ASSERT(!m_pFunc);
#if defined(PLAT_WINDOWS)
		m_pFunc = (F)GetProcAddress(m_pDll, pszSymbol);
		ASSERT_WINDOWS(m_pFunc);
#elif defined(PLAT_LINUX) || defined(PLAT_OSX)
		m_pFunc = (F)dlsym(m_pDll, pszSymbol);
		ASSERT_CUSTOM(m_pFunc, dlerror());
#endif
	}
private:
#if defined(PLAT_LINUX) || defined(PLAT_OSX)
	using HMODULE = void*;
#endif
	typedef R(*F)(Args...);
	HMODULE m_pDll;
	F m_pFunc;
};
