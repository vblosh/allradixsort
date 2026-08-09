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

#include <cstring>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "traits.hpp"

namespace allradixsort
{
	template<class KeyType, class ProxyType>
	inline ProxyType float_flip(const KeyType& value) noexcept
	{
		static_assert(std::is_floating_point<KeyType>::value, "KeyType must be floating point");
		static_assert(std::is_unsigned<ProxyType>::value, "ProxyType must be unsigned");
		static_assert(sizeof(KeyType) == sizeof(ProxyType), "key and proxy sizes must match");
		static_assert(std::numeric_limits<KeyType>::is_iec559, "IEEE-754 floating point is required");

		ProxyType bits{};
		std::memcpy(&bits, &value, sizeof(bits));
		constexpr auto sign_bit = ProxyType{ 1 } << (std::numeric_limits<ProxyType>::digits - 1);
		const auto mask = (bits & sign_bit) != 0 ? ~ProxyType{ 0 } : sign_bit;
		return bits ^ mask;
	}

	template<class KeyType, class Iter, class GetKeyFn>
	void float_sort(Iter begin, Iter end, GetKeyFn get_key)
	{
		using proxy_type = typename traits<KeyType>::proxy_type;
		constexpr auto num_passes = traits<KeyType>::num_passes;
		constexpr auto bits_in_mask = traits<KeyType>::bits_in_mask;
		constexpr auto mask = traits<KeyType>::mask;
		constexpr auto num_bins = traits<KeyType>::num_bins;

		const auto distance = end - begin;
		if (distance <= 1) {
			return;
		}
		const auto size = static_cast<std::size_t>(distance);

		std::vector<std::vector<index_t>> hist(num_passes, std::vector<index_t>(num_bins));
		for (auto it = begin; it != end; ++it) {
			const auto key = float_flip<KeyType, proxy_type>(get_key(*it));
			for (std::size_t pass = 0; pass < num_passes; ++pass) {
				const auto digit = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);
				++hist[pass][digit];
			}
		}

		for (auto& pass_hist : hist) {
			index_t sum = 0;
			for (auto& count : pass_hist) {
				const auto next = sum + count;
				count = sum;
				sum = next;
			}
		}

		std::vector<cont_type_t<Iter>> buffer(size);
		using difference_type = typename std::iterator_traits<Iter>::difference_type;
		for (std::size_t pass = 0; pass < num_passes; ++pass) {
			if ((pass % 2) == 0) {
				for (auto it = begin; it != end; ++it) {
					const auto key = float_flip<KeyType, proxy_type>(get_key(*it));
					const auto digit = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);
					buffer[hist[pass][digit]++] = std::move(*it);
				}
			}
			else {
				for (auto& element : buffer) {
					const auto key = float_flip<KeyType, proxy_type>(get_key(element));
					const auto digit = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);
					*(begin + static_cast<difference_type>(hist[pass][digit]++)) = std::move(element);
				}
			}
		}

		if constexpr ((num_passes % 2) != 0) {
			for (auto& element : buffer) {
				*begin++ = std::move(element);
			}
		}
	}

	template<class Iter>
	void float_sort(Iter begin, Iter end)
	{
		using key_type = cont_type_t<Iter>;
		float_sort<key_type>(begin, end,
			[](const key_type& element) -> const key_type& { return element; });
	}
}
