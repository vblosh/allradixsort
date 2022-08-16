#pragma once
#include <limits>

namespace allradixsort
{
	using index_t = uint32_t;

	template <typename It>
	using cont_type_t = typename std::iterator_traits<It>::value_type;

	template<typename KeyType>
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

	template<typename KeyType, size_t abits_in_mask>
	struct integral_traits
	{
		static constexpr size_t bits_in_mask = abits_in_mask;
		static constexpr size_t num_passes = get_num_bits<KeyType>() % bits_in_mask == 0 ? 
			get_num_passes<KeyType>(bits_in_mask) 
			: get_num_passes<KeyType>(bits_in_mask) + 1;
		static constexpr size_t mask = ~(~0x0u << bits_in_mask);
		static constexpr size_t num_bins = mask + 1;
		static constexpr bool is_integer = std::numeric_limits<KeyType>::is_integer;
		static constexpr bool is_signed_integer = std::numeric_limits<KeyType>::is_signed
			&& std::numeric_limits<KeyType>::is_integer;
		static constexpr bool is_float = std::numeric_limits<KeyType>::is_iec559;
	};

	template<>
	struct traits<uint8_t> : integral_traits< uint8_t, 8> {};

	template<>
	struct traits<uint16_t> : integral_traits< uint16_t, 8> {};
	
	template<>
	struct traits<uint32_t> : integral_traits< uint32_t, 8> {};

	template<>
	struct traits<uint64_t> : integral_traits< uint64_t, 8> {};

	template<>
	struct traits<int8_t> : integral_traits< int8_t, 8> {};

	template<>
	struct traits<int16_t> : integral_traits< int16_t, 8> {};

	template<>
	struct traits<int32_t> : integral_traits< int32_t, 8> {};

	template<>
	struct traits<int64_t> : integral_traits< int64_t, 8> {};

	template<typename KeyType, size_t abits_in_mask>
	struct float_traits : integral_traits<KeyType, abits_in_mask>
	{
		//static_assert(num_passes % 2 == 0, "num_passes should be even");
	};

	template<>
	struct traits<float> : float_traits< float, 8> 
	{
		using proxy_type = uint32_t;
		static constexpr size_t num_passes = 4;
	};

	template<>
	struct traits<double> : float_traits< double, 11> 
	{
		using proxy_type = uint64_t;
		static constexpr size_t num_passes = 6;
	};

}


