#pragma once

namespace gfx {

	class window {
	public:
		virtual const char* get_title() { return "unnamed"; }
		virtual void draw_window() final;

		virtual float min_width() { return -1; }
		virtual float min_height() { return -1; }
	protected:
		virtual void draw_content() {}
	};

}
