#pragma once

#include <gui/imgui.h>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ImGui {
	bool InputFloatN(const char* label, float* v, int components, int decimal_precision = -1, ImGuiInputTextFlags extra_flags = 0);
}

namespace ImGui {
	template<glm::length_t M, glm::length_t N, typename T, glm::qualifier Q>
	void Matrix(const char* szLabel, glm::mat<M, N, T, Q>& mat) {
		ImGui::Text(szLabel);
		T* pData = glm::value_ptr(mat);
		for (glm::length_t i = 0; i < M; i++) {
			ImGui::InputFloatN("", pData + i * N, M);
		}
	}

	template<typename T>
	void Matrix(const char* szLabel, T* pData, glm::length_t M, glm::length_t N) {
		ImGui::Text(szLabel);
		for (glm::length_t i = 0; i < M; i++) {
			ImGui::InputFloatN("", pData + i * N, M);
		}
	}

	template<typename T>
	void Vector(const char* szLabel, T* pData, glm::length_t nSize) {
		ImGui::InputFloatN(szLabel, pData, nSize);
	}
}
