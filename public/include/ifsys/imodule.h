#pragma once

class ifsys;

class imodule {
public:
	// Get interface name
	virtual const char* name() const = 0;
	// Initialize interface
	virtual void init() = 0;
	// Shutdown interface
	virtual void shutdown() = 0;
};
