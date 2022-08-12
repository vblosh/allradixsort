#include <stdint.h>
#include <gtest/gtest.h>
#include <vector>
#include <array>
#include <functional>
#include <random>
#include <limits>
#include <iomanip>

#include "radixsort.h"

namespace allradixsort::tests
{
	using namespace std;

	template<class KeyType>
	using Array = std::vector<std::pair<KeyType, size_t>>;

	constexpr size_t N = 10000;

	template<class KeyType>
	void prepare_data(Array<KeyType>& arr, KeyType min, KeyType max)
	{
		KeyType curmax = std::numeric_limits<KeyType>::min();
		KeyType curmin = std::numeric_limits<KeyType>::max();

		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<double> distr(0.0, 1.0);
		// fill array
		for (size_t i = 0; i < arr.size(); i++)
		{
			arr[i] = { static_cast<KeyType>(distr(eng)*(max - min)	+ min)
				, i };
			if (arr[i].first > curmax) curmax = arr[i].first;
			if (arr[i].first < curmin) curmin = arr[i].first;
		}
		//std::cout << " min=" << curmin << " max=" << curmax  << '\n';
	}

	template<class KeyType>
	void check_data(Array<KeyType>& InArr)
	{
		for (size_t i = 1; i < InArr.size(); ++i)
		{
			ASSERT_FALSE(InArr[i - 1].first > InArr[i].first
				|| (InArr[i - 1].first == InArr[i].first 
					&& InArr[i - 1].second > InArr[i].second));
		}
	}

	TEST(CountingSort, test_it)
	{
		const size_t num_bins = 256;
		using KeyType = uint8_t;
		Array<KeyType> data(N);

		prepare_data<KeyType>(data, std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());
		auto out = counting_sort(data.begin(), data.end(),
			[](const auto& el) { return el.first & 0xff; }
			, num_bins, false);
		check_data(out);
	}

	TEST(CountingSort, test_negative)
	{
		const size_t num_bins = 256;
		using KeyType = int8_t;
		Array<KeyType> data(N);

		prepare_data<KeyType>(data, std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());
		auto out = counting_sort(data.begin(), data.end(),
			[](const auto& el) { return el.first & 0xff; }
		, num_bins, true);
		check_data(out);
	}

	TEST(CountingSort, test_const_it)
	{
		const size_t num_bins = 256;
		Array<uint16_t> data(N);
		prepare_data<uint16_t>(data, 0, num_bins);
		auto out = counting_sort(data.cbegin(), data.cend(),
			[](const auto& el) { return el.first & 0xff; }
		, num_bins, false);
		check_data(out);
	}

	template<typename KeyType>
	void TypeTest(KeyType min, KeyType max)
	{
		Array<KeyType> data(N);
		prepare_data<KeyType>(data, min, max);
		auto out = sort<KeyType>(data.cbegin(), data.cend(),
			[](const auto& el) { return el.first; });
		check_data(out);
	}

	TEST(RadixSort, uint8_t_test)
	{
		using KeyType = uint8_t;
		TypeTest<KeyType>(std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());
	}

	TEST(RadixSort, uint16_t_test)
	{
		using KeyType = uint16_t;
		TypeTest<KeyType>(std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());
	}

	TEST(RadixSort, uint32_t_test)
	{
		using KeyType = uint32_t;
		TypeTest<KeyType>(std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());
	}

	TEST(RadixSort, uint64_t_test)
	{
		using KeyType = uint64_t;
		TypeTest<KeyType>(std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());
	}

	TEST(RadixSort, int8_t_test)
	{
		using KeyType = int8_t;
		TypeTest<KeyType>(std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());
	}

	TEST(RadixSort, int16_t_test)
	{
		using KeyType = int16_t;
		TypeTest<KeyType>(std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());
	}

	TEST(RadixSort, int32_t_test)
	{
		using KeyType = int32_t;
		TypeTest<KeyType>(std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());
	}
	
	TEST(RadixSort, int64_t_test)
	{
		using KeyType = int64_t;
		TypeTest<KeyType>(std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());
	}
	
}