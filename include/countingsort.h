#pragma once
#include <vector>
#include <tuple>
#include <functional>
#include <type_traits>

namespace allradixsort
{
	template <typename It>
	using cont_type_t = typename std::iterator_traits<It>::value_type;

	using index_t = unsigned int;

	// histogram.
	// count each occurrence of indexed-byte value.
	template<class ForwardIt>
	std::pair<std::vector<index_t>, size_t> count_keys_equal(ForwardIt first, ForwardIt last
		, std::function<index_t(const cont_type_t<ForwardIt>&)> get_key, size_t num_bins)
	{
		std::vector<index_t> equals(num_bins);
		size_t size = 0; // we only require to use forward iterator, therefore, we have to find out the size of input container

		using category = typename std::iterator_traits<ForwardIt>::iterator_category;
		if constexpr (std::is_same_v<category, std::random_access_iterator_tag>) {
			size = last - first;
			for (ForwardIt it = first; it != last; ++it)
			{
				++equals[get_key(*it)];
			}
		}
		else {
			for (ForwardIt it = first; it != last; ++it)
			{
				++equals[get_key(*it)];
				++size;
			}
		}
	
		return { equals, size };
	}

	// accumulate.
	// generate positional offsets. adjust starting point if signed.
	std::vector<index_t> count_keys_less(std::vector<index_t>& equals
		, size_t num_bins, bool is_signed)
	{
		std::vector<index_t> less(num_bins);
		
		size_t start = is_signed ? num_bins / 2 : 0;
		for (size_t i = 1 + start; i < equals.size() + start; ++i)
		{
			less[i % num_bins] = less[(i - 1) % num_bins] + equals[(i - 1) % num_bins];
		}
		return less;
	}

	// distribute.
	// stable reordering of elements. backward to avoid shifting
	// the counter array.
	template<class ForwardIt>
	std::vector<cont_type_t<ForwardIt>>
		rearrange(ForwardIt first, ForwardIt last, std::vector<index_t>& less
			, std::function<index_t(const cont_type_t<ForwardIt>&)> get_key, size_t size)
	{
		std::vector<cont_type_t<ForwardIt>> out(size);
		auto& next = less;
		for (ForwardIt it = first; it != last; ++it)
		{
			auto key = get_key(*it);
			auto index = next[key];
			out[index] = *it;
			++next[key];
		}
		return out;
	}

	template<class ForwardIt>
	std::vector<cont_type_t<ForwardIt>> 
		counting_sort(ForwardIt first, ForwardIt last
			, std::function<index_t(const cont_type_t<ForwardIt>&)> get_key
			, size_t num_bins, bool is_signed)
	{
		auto res = count_keys_equal(first, last, get_key, num_bins);
		size_t size = res.second;
		auto less = count_keys_less(res.first, num_bins, is_signed);
		return rearrange(first, last, less, get_key, size);
	}
}

