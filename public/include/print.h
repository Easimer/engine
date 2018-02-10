#pragma once
#include <iostream>
#include <enl/platform.h>

#define PRINT(...) std::cout << __VA_ARGS__ << std::endl;
#define PRINT_ERR(...) std::cerr << __VA_ARGS__ << std::endl;

#ifdef PLAT_DEBUG
	#define PRINT_DBG(...) std::cerr << __VA_ARGS__ << std::endl;
#else /* PLAT_DEBUG */
	#define PRINT_DBG(...) ;
#endif /* PLAT_DEBUG */
