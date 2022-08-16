#pragma once
#include <vector>
#include <array>
#include <functional>
#include <type_traits>

#include "traits.h"

namespace allradixsort
{
	template<class KeyType, class ProxyType>
	ProxyType float_flip(KeyType f);

	template<>
	uint32_t float_flip<float, uint32_t>(float f)
	{
		uint32_t val = *reinterpret_cast<uint32_t*>(&f);
		uint32_t mask = -int32_t(val >> 31) | 0x80000000;
		return val ^ mask;
	};

	template<>
	uint64_t float_flip<double, uint64_t>(double f)
	{
		uint64_t val = *reinterpret_cast<uint64_t*>(&f);
		size_t mask = -int64_t(val >> 63) | 0x8000000000000000;
		return val ^ mask;
	};

	// Sorts [fbegin, fend) using insertion sort with the given key extraction function.
	template<class KeyType, class Iter, class GetKeyFn>
	void float_sort(Iter begin, Iter end, GetKeyFn get_key)
	{
		constexpr size_t num_passes = traits<KeyType>::num_passes;
		constexpr size_t bits_in_mask = traits<KeyType>::bits_in_mask;
		constexpr size_t mask = traits<KeyType>::mask;
		constexpr size_t num_bins = traits<KeyType>::num_bins;
		size_t size = end - begin;

		using hists_vector = std::vector<std::vector<index_t>>;
		// Creating histograms, count each occurrence of indexed-byte value.
		// In particular, histograms don't change when you change the order, 
		// so I just do all the histogramming in one pass through the data. One read builds several histograms.
		hists_vector hist{ num_passes, std::vector<index_t>(num_bins) };
		for (Iter it = begin; it != end; ++it)
		{
			auto key = float_flip<KeyType, typename traits<KeyType>::proxy_type>(get_key(*it));
			for (size_t pass = 0; pass < num_passes; ++pass)
			{
				auto pass_hist_val = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);
				++hist[pass][pass_hist_val];
			}
		}

		// accumulate histograms.
		// generate positional offsets. adjust starting point if signed.
		for (size_t pass = 0; pass < num_passes; ++pass)
		{
			bool is_signed = traits<KeyType>::is_signed_integer
				&& pass == (traits<KeyType>::num_passes - 1);

			index_t tsum, sum = 0;
			size_t start = is_signed ? num_bins / 2 : 0;
			for (size_t i = 0 + start; i < num_bins + start; ++i)
			{
				tsum = hist[pass][i % num_bins] + sum;
				hist[pass][i % num_bins] = sum;
				sum = tsum;
			}
		}

		// distribute.
		// stable reordering of elements. backward to avoid shifting
		// the counter array.
		// temp buffer to hold values in odd passes
		std::vector<cont_type_t<Iter>> buffer(size);

		for (size_t pass = 0; pass < num_passes;)
		{
			for (Iter it = begin; it != end; ++it)
			{
				auto key = float_flip<KeyType, typename traits<KeyType>::proxy_type>(get_key(*it));
				auto pass_hist_val = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);

				auto index = hist[pass][pass_hist_val];


				*(buffer.begin() + index) = *it;
				++hist[pass][pass_hist_val];
			}
			++pass;

			// use input container as a buffer
			for (auto it = buffer.begin(); it != buffer.end(); ++it)
			{
				auto key = float_flip<KeyType, typename traits<KeyType>::proxy_type>(get_key(*it));
				auto pass_hist_val = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);

				auto index = hist[pass][pass_hist_val];
				*(begin + index) = *it;
				++hist[pass][pass_hist_val];
			}
			++pass;
		}
	}

	// Sorts [begin, end) using radix sort 
	template<class Iter>
	void float_sort(Iter begin, Iter end)
	{
		float_sort<cont_type_t<Iter>, Iter>(begin, end, [](const cont_type_t<Iter>& el) { return el; });
	}
}

