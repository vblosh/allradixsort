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

#include <array>
#include <cstring>
#include <functional>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "../traits.hpp"

namespace allradixsort
{
	template<class KeyType>
	constexpr auto normalized_integer_key(KeyType value) noexcept
	{
		static_assert(std::is_integral<KeyType>::value, "KeyType must be integral");
		using unsigned_type = typename std::make_unsigned<KeyType>::type;
		auto key = static_cast<unsigned_type>(value);
		if constexpr (std::is_signed<KeyType>::value) {
			constexpr auto sign_bit = unsigned_type{ 1 }
				<< (std::numeric_limits<unsigned_type>::digits - 1);
			key ^= sign_bit;
		}
		return key;
	}

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

	namespace detail
	{
		template<class KeyType>
		constexpr auto to_radix_key(const KeyType& value) noexcept
		{
			using decayed_key = std::decay_t<KeyType>;
			if constexpr (traits<decayed_key>::is_integer) {
				return normalized_integer_key<decayed_key>(value);
			} else {
				using proxy_type = typename traits<decayed_key>::proxy_type;
				return float_flip<decayed_key, proxy_type>(value);
			}
		}

		template<class KeyType, class Iter, class GetKeyFn>
		void radix_sort_impl(Iter begin, Iter end, GetKeyFn get_key)
		{
			using traits_type = traits<KeyType>;
			constexpr auto num_passes = traits_type::num_passes;
			constexpr auto bits_in_mask = traits_type::bits_in_mask;
			constexpr auto mask = traits_type::mask;
			constexpr auto num_bins = traits_type::num_bins;

			const auto distance = end - begin;
			if (distance <= 1) {
				return;
			}
			const auto size = static_cast<std::size_t>(distance);

			std::array<std::array<index_t, num_bins>, num_passes> hist{};
			for (auto it = begin; it != end; ++it) {
				const auto key = detail::to_radix_key<KeyType>(std::invoke(get_key, *it));
				for (std::size_t pass = 0; pass < num_passes; ++pass) {
					const auto digit = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);
					++hist[pass][digit];
				}
			}

			std::array<bool, num_passes> skip_pass{};
			bool all_skipped = true;
			for (std::size_t pass = 0; pass < num_passes; ++pass) {
				index_t sum = 0;
				for (auto& count : hist[pass]) {
					if (count == size) {
						skip_pass[pass] = true;
						break;
					}
					const auto next = sum + count;
					count = sum;
					sum = next;
				}
				if (!skip_pass[pass]) {
					all_skipped = false;
				}
			}

			if (all_skipped) {
				return;
			}

			std::vector<cont_type_t<Iter>> buffer(size);
			using difference_type = typename std::iterator_traits<Iter>::difference_type;
			bool in_buffer = false;

			for (std::size_t pass = 0; pass < num_passes; ++pass) {
				if (skip_pass[pass]) {
					continue;
				}
				if (!in_buffer) {
					for (auto it = begin; it != end; ++it) {
						const auto key = detail::to_radix_key<KeyType>(std::invoke(get_key, *it));
						const auto digit = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);
						buffer[hist[pass][digit]++] = std::move(*it);
					}
					in_buffer = true;
				}
				else {
					for (auto& element : buffer) {
						const auto key = detail::to_radix_key<KeyType>(std::invoke(get_key, element));
						const auto digit = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);
						*(begin + static_cast<difference_type>(hist[pass][digit]++)) = std::move(element);
					}
					in_buffer = false;
				}
			}

			if (in_buffer) {
				for (auto& element : buffer) {
					*begin++ = std::move(element);
				}
			}
		}
	}
}
