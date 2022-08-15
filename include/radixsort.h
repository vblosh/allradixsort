#pragma once
#include <vector>
#include <array>
#include <functional>
#include <type_traits>

#include "traits.h"

namespace allradixsort
{
	template<class KeyType, class ForwardIt, class GetKeyFn>
	void sort(ForwardIt first, ForwardIt last, GetKeyFn get_key)
	{
		constexpr size_t num_passes = traits<KeyType>::num_passes;
		constexpr size_t bits_in_mask = traits<KeyType>::bits_in_mask;
		constexpr size_t mask = ~(~0x0u << bits_in_mask);
		constexpr size_t num_bins = mask + 1;
		size_t size = last - first;

		using hists_vector = std::vector<std::vector<index_t>>;
		// Creating histograms, count each occurrence of indexed-byte value.
		// In particular, histograms don't change when you change the order, 
		// so I just do all the histogramming in one pass through the data. One read builds several histograms.
		hists_vector equals{ num_passes, std::vector<index_t>(num_bins) };
		for (ForwardIt it = first; it != last; ++it)
		{
			for (size_t pass = 0; pass < num_passes; ++pass)
			{
				KeyType key = get_key(*it);
				auto pass_hist_val = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);
				++equals[pass][pass_hist_val];
			}
		}

		// accumulate histograms.
		// generate positional offsets. adjust starting point if signed.
		hists_vector less{ num_passes, std::vector<index_t>(num_bins) };
		for (size_t pass = 0; pass < num_passes; ++pass)
		{
			bool is_signed = std::numeric_limits<KeyType>::is_signed
				&& pass == (traits<KeyType>::num_passes - 1);

			size_t start = is_signed ? num_bins / 2 : 0;
			for (size_t i = 1 + start; i < num_bins + start; ++i)
			{
				less[pass][i % num_bins] = less[pass][(i - 1) % num_bins] + equals[pass][(i - 1) % num_bins];
			}
		}

		// distribute.
		// stable reordering of elements. backward to avoid shifting
		// the counter array.
		// temp buffer to hold values in odd passes
		cont_type_t<ForwardIt>* buffer = static_cast<cont_type_t<ForwardIt>*>(malloc(sizeof(cont_type_t<ForwardIt>) * size));

		for (size_t pass = 0; pass < num_passes;)
		{
			for (ForwardIt it = first; it != last; ++it)
			{
				KeyType key = get_key(*it);
				auto pass_hist_val = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);

				auto index = less[pass][pass_hist_val];
				if (pass == 0) {
					new(buffer + index) cont_type_t<ForwardIt>(*it);
				}
				else {
					*(buffer + index) = *it;
				}
				++less[pass][pass_hist_val];
			}
			++pass;
			if (pass == num_passes) {
				// copy values back to input container
				std::copy(buffer, buffer + size, first);
			}
			else {
				// use input container as a buffer
				for (auto it = buffer; it != buffer + size; ++it)
				{
					auto key = get_key(*it);
					auto pass_hist_val = static_cast<index_t>((key >> (bits_in_mask * pass)) & mask);

					auto index = less[pass][pass_hist_val];
					*(first + index) = *it;
					++less[pass][pass_hist_val];
				}
			}
			++pass;
		}
		free(buffer);
	}
}

