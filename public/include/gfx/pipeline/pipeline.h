#pragma once

#include <memory>
#include <vector>
#include <gfx/pipeline/intake.h>
#include <gfx/pipeline/intermediate.h>
#include <gfx/pipeline/delivery.h>

namespace gfx::pipeline {

struct draw_order {
	gfx::model_id model;
	size_t shader;
	//size_t material; // TODO: ref to mat obj can't be queried through gpGfx
	const glm::mat4& mat_trans;
	const glm::mat4& mat_view;
	const glm::mat4& mat_proj;
};

class pipeline {
public:
	pipeline(const std::string& filename);
	void set_intake(const intake&);
	void set_delivery(const delivery&);
	void push_stage(const intermediate&);

	void begin();
	void draw(const draw_order&);
	void finalize();
private:
	intake m_intake;
	delivery m_delivery;
	std::vector<intermediate> m_stages;
};

}
