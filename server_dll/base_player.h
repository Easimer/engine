#pragma once

#include "base_entity.h"
#include "prop_common.h"

class base_player : public c_base_prop {
public:
	DEC_CLASS(base_player, c_base_prop);
	virtual ~base_player() {};

	virtual void precache() override;
	virtual void spawn() override;

	virtual inline bool networked() const final { return true; }
	virtual bool is_player() const final { return true; }
	virtual bool is_drawable() final{ return true; }

	size_t get_client_handle() const { return m_iAssignedClient; }
	void set_client_handle(size_t iHandle) { m_iAssignedClient = iHandle; }

private:
	size_t m_iAssignedClient;
};
