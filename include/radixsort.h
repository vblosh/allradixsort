#pragma once
#include "countingsort.h"
#include "traits.h"

namespace allradixsort
{
	template<class KeyType, class ForwardIt>
	std::vector<cont_type_t<ForwardIt>>
		sort(ForwardIt first, ForwardIt last, std::function<KeyType(const cont_type_t<ForwardIt>&)> get_key)
	{
		std::vector<cont_type_t<ForwardIt>> result = counting_sort(first, last
			, traits<KeyType, ForwardIt>::get_pass_key(0, get_key)
			, traits<KeyType, ForwardIt>::get_num_bins(0)
			, std::numeric_limits<KeyType>::is_signed && traits<KeyType, ForwardIt>::num_passes == 1);
		
		for (size_t i = 1; i < traits<KeyType, ForwardIt>::num_passes; i++)		{
			result = counting_sort(result.cbegin(), result.cend()
				, traits<KeyType, ForwardIt>::get_pass_key(i, get_key)
				, traits<KeyType, ForwardIt>::get_num_bins(i)
				, std::numeric_limits<KeyType>::is_signed && i == (traits<KeyType, ForwardIt>::num_passes - 1));
		}
		return result;
	}
}

