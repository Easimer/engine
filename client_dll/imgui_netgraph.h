#pragma once

#include <vector>
#include <cstdint>
#include <net/networking.h>
#include <ring_buffer.h>

namespace ImGui {
	void NetGraph(const std::vector<net::packet_stat>&);
	template<size_t N>
	void NetGraph(ring_buffer<net::packet_stat, N>& buf) {
		if (buf.empty())
			return;
		auto aStat = buf.ptr();
		auto iHead = buf.head();
		auto iTail = buf.tail();
		// [iTail; iHead[
		std::vector<net::packet_stat> stat;
		if (iHead > iTail) { // Simple
			for (size_t i = iTail; i < iHead; i++) {
				stat.push_back(aStat[i]);
			}
		}
		else if (iHead <= iTail) { // Warp around
			for (size_t i = iTail; i < N; i++) {
				stat.push_back(aStat[i]);
			}
			for (size_t i = 0; i < iHead; i++) {
				stat.push_back(aStat[i]);
			}
		}
		NetGraph(stat);
	}
}
