#pragma once
/*
 * Copyright (c) 2020 Vyacheslav Bloshchanevich
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <vector>
#include <array>
#include <functional>
#include <type_traits>

#include "traits.hpp"
#include "integersort.hpp"
#include "floatsort.hpp"

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

