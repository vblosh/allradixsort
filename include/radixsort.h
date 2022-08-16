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
			static_assert(false, "this key type is not supported");
		}
	}

	// Sorts [begin, end) using radix sort 
	template<class Iter>
	void sort(Iter begin, Iter end)
	{
		sort<cont_type_t<Iter>, Iter>(begin, end, [](const cont_type_t<Iter>& el) { return el; });
	}
}

