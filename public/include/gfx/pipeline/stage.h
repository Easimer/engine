#pragma once

namespace gfx::pipeline {

class stage {
public:
	stage(bool ready = false) : m_ready(ready) {}
	// Is stage ready
	operator bool() { return m_ready; };
protected:
	bool m_ready;
};

}
