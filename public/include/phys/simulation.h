#pragma once

#include <phys/object.h>
#include <vector>

namespace phys {
	class simulation {
	public:
		void add_object(const phys::object& obj);
		void simulate(float delta);
	private:
		std::vector<phys::object> m_objects;
	};
}
