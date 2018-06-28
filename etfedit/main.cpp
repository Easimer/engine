#include "stdafx.h"
#include <gfx/gfx.h>
#include <gfx/etf.h>
#include <SDL2/SDL_image.h>

#ifdef PLAT_WINDOWS
#include <Windows.h>
#endif

struct dir_entry {
	bool bDirectory;
	bool bUp;
	std::string filename;
	size_t size;
};

std::vector<dir_entry> directory_listing;
long long selected_entry = -1;
gfx::shared_tex2d current_texture;

std::vector<dir_entry> list_files(const std::vector<std::string>& path_stack) {
	std::vector<dir_entry> ret;
	// Add implicit entry "Up" if the current dir is not "/"
	if (path_stack.size() != 0) {
		dir_entry e_up;
		e_up.bDirectory = e_up.bUp = true;
		e_up.filename = "..";
		ret.push_back(e_up);
	}
	// Win32 implementation
#if defined(PLAT_WINDOWS)
	WIN32_FIND_DATA ffd;
	HANDLE hFile;
	std::string path = "data/";
	for (const auto& dir : path_stack) {
		path.append(dir);
		path.append("/");
	}
	path.append("*");
	hFile = FindFirstFileA(path.c_str(), &ffd);
	while (hFile != INVALID_HANDLE_VALUE) {
		if (ffd.cFileName[0] != '.') {
			dir_entry e;
			e.bDirectory = ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			e.bUp = false;
			e.filename = std::string(ffd.cFileName);
			e.size = ((uint64_t)ffd.nFileSizeHigh << 32) | ffd.nFileSizeLow;
			ret.push_back(e);
		}
		if (FindNextFileA(hFile, &ffd) == 0)
			break;
	}
	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES) {
		//PRINT_DBG(dwError);
	}
	FindClose(hFile);
#endif
	return ret;
}

// save - file save variant
// Returns buf if the user has selected a filename, nullptr otherwise
bool file_dialog(std::vector<std::string>& path_stack, bool save) {
	ImVec2 wsiz(560, 240);
	bool ret = false;
	ImGui::SetNextWindowSize(wsiz);
	if (ImGui::Begin(save ? "Save file" : "Open file", nullptr, ImGuiWindowFlags_NoSavedSettings)) {
		std::string path = "/data/";
		std::array<char, 128> selected_file;
		for (const auto& pelem : path_stack) {
			path.append(pelem);
			path.append("/");
		}
		ImGui::InputText("##empty", (char*)path.c_str(), path.size(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine(); ImGui::Button("Up");
		if (ImGui::ListBoxHeader("##empty")) {
			for (size_t i = 0; i < directory_listing.size(); i++) {
				const auto& file = directory_listing[i];
				if (ImGui::Selectable(file.filename.c_str(), i == selected_entry)) {
					if (file.bDirectory) {
						path_stack.push_back(file.filename);
						directory_listing = list_files(path_stack);
						selected_entry = -1;
					} else {
						selected_entry = i;
					}
				}
			}
			ImGui::ListBoxFooter();
		}
		if(selected_entry != -1)
			strncpy(selected_file.data(), directory_listing[selected_entry].filename.c_str(), selected_file.size());
		ImGui::InputText("##empty", selected_file.data(), selected_file.size());
		ImGui::SameLine();
		if (ImGui::Button(save ? "Save" : "Open")) {
			path_stack.push_back(std::string(selected_file.data()));
			ret = true;
		}
	}
	ImGui::End();
	return ret;
}

void load_image(std::vector<std::string>& path_stack) {
	std::string path = "data/";
	std::string filename = path_stack.back();
	path_stack.pop_back();
	for (const auto& dir : path_stack) {
		path.append(dir);
		path.append("/");
	}
	path.append(filename);

	current_texture = gpGfx->load_texture(path);
	current_texture->generate_mipmap();
}

#undef main
int main() {
	gpGfx->init("Engine Texture Format Editor");
	bool bShutdown = false;
	// A file is loaded
	bool bLoaded = false;
	bool bOpenFileDialogOpen = false;
	bool bSaveFileDialogOpen = false;
	std::vector<std::string> path;

	while (!bShutdown) {
		if (gpGfx->handle_events()) bShutdown = true;

		gpGfx->begin_frame();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Import", "Ctrl-O")) {
					bOpenFileDialogOpen = true;
					bSaveFileDialogOpen = false;
					directory_listing = list_files(path);
					selected_entry = -1;
				}
				if (ImGui::MenuItem("Save", "Ctrl-S", nullptr, bLoaded)) {
					bOpenFileDialogOpen = false;
					bSaveFileDialogOpen = true;
					directory_listing = list_files(path);
					selected_entry = -1;
				}
				if (ImGui::MenuItem("Quit")) {
					bShutdown = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (bOpenFileDialogOpen) {
			if (file_dialog(path, false)) {
				bOpenFileDialogOpen = false;
				load_image(path);
				bLoaded = true;
			}
		}
		if (bSaveFileDialogOpen) {
			if (file_dialog(path, true)) {
				bSaveFileDialogOpen = false;
				gfx::etf::etf_writer wr;
				bLoaded = false;
			}
		}

		if (current_texture) {
			ImVec2 win_siz(480, 480);
			ImGui::SetNextWindowSize(win_siz);
			ImGui::SetNextWindowSizeConstraints(win_siz, win_siz);
			if (ImGui::Begin("Texture View", nullptr, ImGuiWindowFlags_NoSavedSettings)) {
				ImVec2 tex_siz(432, 432);
				ImGui::Image((ImTextureID)(uint64_t)current_texture->handle(), tex_siz);
			}
			ImGui::End();
		}

		gpGfx->end_frame();
	}


	gpGfx->shutdown();
	return 0;
}
