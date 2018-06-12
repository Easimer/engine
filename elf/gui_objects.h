#pragma once

#include <ifsys/ifsys.h>
#include <elf/imapeditor.h>
#include <gfx/gfx.h>
#include <gfx/window.h>
#include <vector>
#include <string>
#include <map>

// A window that shows all placeable models
// Users can navigate the directory structure
// All models have preview icons that are generated
// when the user opens a directory
class gui_objects : public gfx::window {
public:
	virtual ~gui_objects() {
	}
	virtual const char* get_title() override { return "Objects"; }
	virtual float min_width() override { return gpGfx->width(); }
	virtual float min_height() override { return 300; }
	void set_is(ifsys* is);

protected:
	virtual void draw_content() override;

	struct dir_entry {
		bool bDirectory;
		bool bUp;
		std::string filename;
	};

	// Returns a list of files and directories
	std::vector<dir_entry> list_files() const;

	const std::string cwd() const {
		std::string path = "data/models/";
		for (const auto& dir : m_stack_path) {
			path.append(dir);
			path.append("/");
		}
		return path;
	}

	gfx::shared_tex2d generate_preview(gfx::model_id id);
private:
	std::vector<std::string> m_stack_path;
	std::vector<dir_entry> m_cur_entries;

	std::map<std::string, gfx::model_id> m_object_cache;
	std::map<gfx::model_id, gfx::shared_tex2d> m_preview_cache;

	ifsys* m_pISys;
	imapeditor* m_pMapEditor = nullptr;
};