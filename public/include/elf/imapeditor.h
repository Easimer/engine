#pragma once

#include <ifsys/imodule.h>

class imapeditor : public imodule {
public:
	virtual const char * name() const override = 0;
	virtual void init() override = 0;
	virtual bool shutdown() override = 0;
};
