#pragma once

#include <enl/platform.h>

#if defined(PLAT_WINDOWS)
#include <limits.h>
#include <intrin.h>
#elif defined(PLAT_LINUX)
#include <cpuid.h>
#endif

namespace cpu {

	class cpuid {
	public:
		cpuid(unsigned i) {
#if defined(PLAT_WINDOWS)
			__cpuid((int*)m_regs, i);
#elif (defined(PLAT_LINUX) || defined(PLAT_OSX)) && (__GNUG__ || __clang__)
			__cpuid(i, m_regs[0], m_regs[1], m_regs[2], m_regs[3]);
#else
			// On unknown platforms, assume that everything is supported and hope for the best
			m_regs[0] = m_regs[1] = m_regs[2] = m_regs[3] = 0xffffffff;
#endif
		}

		auto eax() const { return m_regs[0]; }
		auto ebx() const { return m_regs[1]; }
		auto ecx() const { return m_regs[2]; }
		auto edx() const { return m_regs[3]; }
	private:
		uint32_t m_regs[4];
	};

	namespace features {

		const uint32_t flag_sse2 = 0x4000000; // EDX
		const uint32_t flag_sse3 = 0x1; // ECX
		const uint32_t flag_sse41 = 0x80000; // ECX
		const uint32_t flag_sse42 = 0x100000; // ECX
		const uint32_t flag_avx = 0x10000000; // ECX
		const uint32_t flag_avx2 = 0x20; // EBX

		inline bool sse2() {
			return cpu::cpuid(1).edx() & flag_sse2;
		}

		inline bool sse3() {
			return cpu::cpuid(1).ecx() & flag_sse3;
		}

		inline bool sse41() {
			return cpu::cpuid(1).ecx() & flag_sse41;
		}

		inline bool sse42() {
			return cpu::cpuid(1).ecx() & flag_sse42;
		}

		inline bool avx() {
			return cpu::cpuid(1).ecx() & flag_avx;
		}
		
		inline bool avx2() {
			return cpu::cpuid(7).ebx() & flag_avx2;
		}
	}

}
