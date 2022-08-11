#pragma once
#include <vector>
#include <tuple>
#include <functional>

namespace allradixsort
{
	template <typename It>
	using cont_type_t = std::remove_cv_t<std::remove_reference_t<decltype(*std::declval<It>())>>;

	using index_t = unsigned int;

	template<class ForwartIt>
	std::pair<std::vector<index_t>, size_t> count_keys_equal(ForwartIt first, ForwartIt last, std::function<index_t(const cont_type_t<ForwartIt>&)> get_key, size_t MAX)
	{
		std::vector<index_t> equals(MAX + 1);
		size_t size = 0; // we only require to use forward iterator, therefore, we have to find out the size of input container
		for (ForwartIt it = first; it != last; ++it)
		{
			++equals[get_key(*it)];
			++size;
		}
		return { equals, size };
	}

	std::vector<index_t> count_keys_less(std::vector<index_t>& equals, size_t MAX)
	{
		std::vector<index_t> less(MAX + 1);
		for (size_t i = 1; i < equals.size(); ++i)
		{
			less[i] = less[i - 1] + equals[i - 1];
		}
		return less;
	}

	template<class ForwartIt>
	std::vector<cont_type_t<ForwartIt>>
		rearrange(ForwartIt first, ForwartIt last, std::vector<index_t>& less, std::function<index_t(const cont_type_t<ForwartIt>&)> get_key, size_t size)
	{
		std::vector<cont_type_t<ForwartIt>> out(size);
		auto& next = less;
		for (ForwartIt it = first; it != last; ++it)
		{
			auto key = get_key(*it);
			auto index = next[key];
			out[index] = *it;
			++next[key];
		}
		return out;
	}

	template<class ForwartIt>
	std::vector<cont_type_t<ForwartIt>> 
		counting_sort(ForwartIt first, ForwartIt last, std::function<index_t(const cont_type_t<ForwartIt>&)> get_key, size_t MAX)
	{
		auto res = count_keys_equal(first, last, get_key, MAX);
		size_t size = res.second;
		auto less = count_keys_less(res.first, MAX);
		return rearrange(first, last, less, get_key, size);
	}
}

