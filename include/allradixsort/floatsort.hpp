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

#include <type_traits>
#include <utility>

#include "traits.hpp"
#include "detail/sort_impl.hpp"

namespace allradixsort
{
	template<
		class KeyType = void,
		class Iter,
		class GetKeyFn,
		class ActualKeyType = std::conditional_t<
			std::is_void_v<KeyType>,
			std::decay_t<std::invoke_result_t<GetKeyFn&, decltype(* std::declval<Iter>())>>,
			std::decay_t<KeyType>>>
	void float_sort(Iter begin, Iter end, GetKeyFn get_key)
	{
		static_assert(traits<ActualKeyType>::is_float, "KeyType must be floating point");
		detail::radix_sort_impl<ActualKeyType>(begin, end, get_key);
	}

	template<
		class KeyType = void,
		class Iter,
		class ActualKeyType = std::conditional_t<
			std::is_void_v<KeyType>,
			cont_type_t<Iter>,
			std::decay_t<KeyType>>>
	void float_sort(Iter begin, Iter end)
	{
		static_assert(std::is_same_v<ActualKeyType, cont_type_t<Iter>>,
			"KeyType must match container value_type for scalar sort");
		float_sort<ActualKeyType>(begin, end,
			[](const auto& element) noexcept -> const auto& { return element; });
	}
}
