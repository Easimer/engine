#pragma once

namespace gfx {

	class window {
	public:
		virtual const char* get_title() { return "unnamed"; }
		virtual void draw_window() final;
	protected:
		virtual void draw_content() {}
	};

}
