#pragma once
#include <vector>
#include <array>
#include <functional>
#include <type_traits>

#include "traits.hpp"
#include "floatsort.hpp"

namespace allradixsort
{
	// Sorts [begin, end) using insertion sort with the given key extraction function.
	template<class KeyType, class Iter, class GetKeyFn>
	void integer_sort(Iter begin, Iter end, GetKeyFn get_key)
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
			auto key = get_key(*it);
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
			bool is_signed_and_last_pass = traits<KeyType>::is_signed_integer
				&& pass == (num_passes - 1);

			index_t tsum, sum = 0;
			if (is_signed_and_last_pass) {
				size_t cur_num_bins = (0x1u << (traits<KeyType>::num_bits - (num_passes - 1) * bits_in_mask));
				size_t start = cur_num_bins / 2;
				for (size_t i = 0 + start; i < cur_num_bins + start; ++i)
				{
					tsum = hist[pass][i % cur_num_bins] + sum;
					hist[pass][i % cur_num_bins] = sum;
					sum = tsum;
				}
			}
			else {
				for (size_t i = 0; i < num_bins; ++i)
				{
					tsum = hist[pass][i] + sum;
					hist[pass][i] = sum;
					sum = tsum;
				}
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
				auto key = get_key(*it);
				auto pass_hist_val = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);

				auto index = hist[pass][pass_hist_val];
				*(buffer.begin() + index) = std::move(*it);
				++hist[pass][pass_hist_val];
			}
			++pass;
			if (pass == num_passes) {
				// if num_passes is odd, copy values back to input container on last pass
				for (auto src_it = buffer.begin(), dst_it = begin; src_it != buffer.end(); ++src_it)
				{
					*(dst_it++) = std::move(*src_it);
				}
			}
			else {
				// use input container as a buffer
				for (auto it = buffer.begin(); it != buffer.end(); ++it)
				{

					auto key = get_key(*it);
					auto pass_hist_val = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);

					auto index = hist[pass][pass_hist_val];
					*(begin + index) = std::move(*it);
					++hist[pass][pass_hist_val];
				}
			}
			++pass;
		}
	}
}

