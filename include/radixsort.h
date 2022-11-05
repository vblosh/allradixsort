#pragma once
#include <vector>
#include <array>
#include <functional>
#include <type_traits>

#include "traits.h"
#include "integersort.h"
#include "floatsort.h"

namespace allradixsort
{
	template<class>
	inline constexpr bool dependent_false_v = false;

	// Sorts [begin, end) using insertion sort with the given key extraction function.
	template<class KeyType, class Iter, class GetKeyFn>
	void sort(Iter begin, Iter end, GetKeyFn get_key)
	{
		if constexpr (traits<KeyType>::is_integer) {
			integer_sort<KeyType, Iter, GetKeyFn>(begin, end, get_key);
		}
		else if constexpr (traits<KeyType>::is_float) {
			float_sort<KeyType, Iter, GetKeyFn>(begin, end, get_key);
		}
		else {
			static_assert(dependent_false_v<KeyType>, "this key type is not supported");
		}
	}

	// Sorts [begin, end) using radix sort 
	template<class Iter>
	void sort(Iter begin, Iter end)
	{
		sort<cont_type_t<Iter>, Iter>(begin, end, [](cont_type_t<Iter>& el) ->cont_type_t<Iter>&{ return el; });
	}
}

