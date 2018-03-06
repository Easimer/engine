#pragma once

#include <string>
#include <model.h>

class emf_loader {
public:
	emf_loader() {}
	emf_loader(const std::string& filename);
	model get_model() const;

private:
	model m_model;
};
