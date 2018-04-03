#pragma once

#include <vector>
#include <map>
#include <type_traits>

#include <gfx/window.h>

namespace gfx {
	class iwindow_factory {
	public:
		virtual gfx::window* create() = 0;
	};

	template<typename T>
	class window_factory : public iwindow_factory {
	public:
		window_factory(const std::string& name) {
			gpWindowRegister->emplace(name, this);
		}
		virtual T* create() override {
			return new T;
		}

		static_assert(std::is_base_of<gfx::window, T>::value);
	};

	class window_register {
	public:
		window_register() {}
		void operator=(const window_register&) = delete;
		window_register(const window_register&) = delete;

		~window_register() {
			delete m_factories;
			for (auto& w : m_created_windows) {
				delete w;
			}
		}

		void emplace(const std::string& key, gfx::iwindow_factory* value) {
			if (!m_factories)
				m_factories = new std::map<std::string, gfx::iwindow_factory*>;
			m_factories->emplace(key, value);
		}

		gfx::window* create(const std::string& name) {
			if (!m_factories)
				m_factories = new std::map<std::string, gfx::iwindow_factory*>;
			if (m_factories->count(name)) {
				auto ret = m_factories->operator[](name)->create();
				m_created_windows.push_back(ret);
				return ret;
			}
			return nullptr;
		}
	private:
		std::map<std::string, gfx::iwindow_factory*>* m_factories = nullptr;
		std::vector<gfx::window*> m_created_windows;
	};

	extern gfx::window_register* gpWindowRegister;
}

#define REGISTER_WINDOW(classname, name) static gfx::window_factory<classname> wfactory_##classname(name);
