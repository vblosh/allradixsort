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

#include <limits>

namespace allradixsort
{
	using index_t = uint32_t;

	template <typename It>
	using cont_type_t = typename std::iterator_traits<It>::value_type;

	template<typename KeyType>
	struct traits;

	template<typename KeyType, size_t anum_bits, size_t abits_in_mask>
	struct integral_traits
	{
		static constexpr size_t num_bits = anum_bits;
		static constexpr size_t bits_in_mask = abits_in_mask;
		static constexpr size_t num_passes = num_bits % bits_in_mask == 0 ?
			num_bits / bits_in_mask
			: num_bits / bits_in_mask + 1;
		static constexpr size_t mask = ~(~0x0u << bits_in_mask);
		static constexpr size_t num_bins = mask + 1;
		static constexpr bool is_integer = std::numeric_limits<KeyType>::is_integer;
		static constexpr bool is_signed_integer = std::numeric_limits<KeyType>::is_signed
			&& std::numeric_limits<KeyType>::is_integer;
		static constexpr bool is_float = std::numeric_limits<KeyType>::is_iec559;
//		static_assert((is_signed_integer && bits_in_mask == 8) || !is_signed_integer, "by signed integer num_bits should be 8");
	};

	template<>
	struct traits<uint8_t> : integral_traits< uint8_t, 8, 4> {};

	template<>
	struct traits<uint16_t> : integral_traits< uint16_t, 16, 8> {};
	
	template<>
	struct traits<uint32_t> : integral_traits< uint32_t, 32, 8> {};

	template<>
	struct traits<uint64_t> : integral_traits< uint64_t, 64, 8> {};

	template<>
	struct traits<int8_t> : integral_traits< int8_t, 8, 4> {};

	template<>
	struct traits<int16_t> : integral_traits< int16_t, 16, 8> {};

	template<>
	struct traits<int32_t> : integral_traits< int32_t, 32, 8> {};

	template<>
	struct traits<int64_t> : integral_traits< int64_t, 64, 8> {};

	template<>
	struct traits<float> : integral_traits< float, 32, 8>
	{
		using proxy_type = uint32_t;
		static_assert(num_passes % 2 == 0, "num_passes should be even");
	};

	template<>
	struct traits<double> : integral_traits< double, 64, 11>
	{
		using proxy_type = uint64_t;
		static_assert(num_passes % 2 == 0, "num_passes should be even");
	};

}


