#pragma once
#include <vector>
#include <array>
#include <functional>
#include <type_traits>

#include "traits.h"

namespace allradixsort
{
	// Sorts [begin, end) using insertion sort with the given key extraction function.
	template<class KeyType, class Iter, class GetKeyFn>
	void sort(Iter begin, Iter end, GetKeyFn get_key)
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
			size_t key = get_key(*it);
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
				KeyType key = get_key(*it);
				auto pass_hist_val = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);

				auto index = hist[pass][pass_hist_val];
				*(buffer.begin() + index) = *it;
				++hist[pass][pass_hist_val];
			}
			++pass;
			if (pass == num_passes) {
				// if num_passes is odd, copy values back to input container on last pass
				std::copy(buffer.begin(), buffer.end(), begin);
			}
			else {
				// use input container as a buffer
				for (auto it = buffer.begin(); it != buffer.end(); ++it)
				{
					auto key = get_key(*it);
					auto pass_hist_val = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);

					auto index = hist[pass][pass_hist_val];
					*(begin + index) = *it;
					++hist[pass][pass_hist_val];
				}
			}
			++pass;
		}
	}

	// Sorts [begin, end) using radix sort 
	template<class Iter>
	void sort(Iter begin, Iter end)
	{
		sort<cont_type_t<Iter>, Iter>(begin, end, [](const cont_type_t<Iter>& el) { return el; });
	}
}

