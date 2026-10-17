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

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <type_traits>

namespace allradixsort
{
	using index_t = std::size_t;

	template <typename It>
	using cont_type_t = typename std::iterator_traits<It>::value_type;

	template<typename KeyType, typename = void>
	struct traits
	{
		static constexpr bool is_integer = false;
		static constexpr bool is_signed_integer = false;
		static constexpr bool is_float = false;
	};

	template<typename KeyType, std::size_t NumBits, std::size_t BitsInMask>
	struct integral_traits
	{
		static_assert(BitsInMask > 0, "a radix digit must contain at least one bit");
		static_assert(BitsInMask < std::numeric_limits<std::size_t>::digits,
			"the radix digit is too wide for size_t");

		static constexpr std::size_t num_bits = NumBits;
		static constexpr std::size_t bits_in_mask = BitsInMask;
		static constexpr std::size_t num_passes = (num_bits + bits_in_mask - 1) / bits_in_mask;
		static constexpr std::size_t num_bins = std::size_t{ 1 } << bits_in_mask;
		static constexpr std::size_t mask = num_bins - 1;
		static constexpr bool is_integer = std::numeric_limits<KeyType>::is_integer;
		static constexpr bool is_signed_integer = std::numeric_limits<KeyType>::is_signed
			&& std::numeric_limits<KeyType>::is_integer;
		static constexpr bool is_float = std::numeric_limits<KeyType>::is_iec559;
	};

	template<typename KeyType>
	struct traits<KeyType, std::enable_if_t<std::is_integral_v<KeyType> && !std::is_same_v<KeyType, bool>>>
		: integral_traits<KeyType, sizeof(KeyType) * 8, 8> {};

	template<>
	struct traits<float> : integral_traits<float, 32, 8>
	{
		using proxy_type = std::uint32_t;
	};

	template<>
	struct traits<double> : integral_traits<double, 64, 8>
	{
		using proxy_type = std::uint64_t;
	};

	template<> struct traits<const float> : traits<float> {};
	template<> struct traits<const double> : traits<double> {};
}
