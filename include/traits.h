#pragma once
#include <limits>
#include "countingsort.h"

namespace allradixsort
{
	template<typename KeyType, typename ForwardIt>
	struct traits;

	template<typename KeyType>
	constexpr size_t get_num_bits()
	{
		return (std::numeric_limits<KeyType>::is_signed) ?
			std::numeric_limits<KeyType>::digits + 1 :
			std::numeric_limits<KeyType>::digits;
	}

	template<typename KeyType>
	constexpr size_t get_num_passes(size_t bits_in_mask)
	{
		return get_num_bits<KeyType>() / bits_in_mask;
	}

	template<typename KeyType, size_t bits_in_mask, typename ForwardIt>
	struct integral_traits
	{
		static constexpr size_t mask = ~(~0x0u << bits_in_mask);

		static_assert(get_num_bits<KeyType>() % bits_in_mask == 0, "can not calculate num_passes");
		static constexpr size_t num_passes = get_num_passes<KeyType>(bits_in_mask);
		static_assert(num_passes > 0, "can not calculate num_passes");

		static std::function<index_t(const cont_type_t<ForwardIt>&)>
			get_pass_key(size_t pass, std::function<KeyType(const cont_type_t<ForwardIt>&)> get_key)
		{
			return [get_key, pass](const cont_type_t<ForwardIt>& item) 
					{ return static_cast<index_t>((get_key(item) >> (bits_in_mask * pass)) & mask); };
		}

		static size_t get_num_bins(size_t pass)
		{
			return mask + 1;
		}
	};

	template<typename ForwardIt>
	struct traits<uint8_t, ForwardIt> : integral_traits< uint8_t, 8, ForwardIt> {};

	template<typename ForwardIt>
	struct traits<uint16_t, ForwardIt> : integral_traits< uint16_t, 16, ForwardIt> {};

	template<typename ForwardIt>
	struct traits<uint32_t, ForwardIt> : integral_traits< uint32_t, 16, ForwardIt> {};

	template<typename ForwardIt>
	struct traits<uint64_t, ForwardIt> : integral_traits< uint64_t, 16, ForwardIt> {};

	template<typename ForwardIt>
	struct traits<int8_t, ForwardIt> : integral_traits< int8_t, 8, ForwardIt> {};

	template<typename ForwardIt>
	struct traits<int16_t, ForwardIt> : integral_traits< int16_t, 16, ForwardIt> {};

	template<typename ForwardIt>
	struct traits<int32_t, ForwardIt> : integral_traits< int32_t, 16, ForwardIt> {};

	template<typename ForwardIt>
	struct traits<int64_t, ForwardIt> : integral_traits< int64_t, 16, ForwardIt> {};
}


