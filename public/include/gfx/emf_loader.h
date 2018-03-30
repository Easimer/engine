#pragma once

#include <string>
#include <gfx/model.h>

namespace gfx {

	class emf_loader {
	public:
		emf_loader() {}
		emf_loader(const std::string& filename);
		gfx::model get_model() const;

	private:
		gfx::model m_model;
	};

}
