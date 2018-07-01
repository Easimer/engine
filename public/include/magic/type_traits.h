#pragma once

#include <type_traits>

namespace mgc {
	
	template<typename T, typename = void>
	struct is_less_than_comparable : std::false_type {};

	template<typename T>
	struct is_less_than_comparable
	<T,
	typename std::enable_if<
		true, decltype(std::declval<T&>() < std::declval<T&>(), (void)0)>::type
	> : std::true_type {};
}

