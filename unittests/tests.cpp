#include <stdint.h>
#include <gtest/gtest.h>
#include <vector>
#include <random>
#include <limits>
#include <iomanip>

#include "radixsort.h"

namespace allradixsort
{
namespace tests
{
	using namespace std;

	template<class KeyType>
	using Array = std::vector<std::pair<KeyType, size_t>>;

	constexpr size_t N = 10000;

	template<class KeyType>
	void prepare_data(Array<KeyType>& arr, KeyType min, KeyType max)
	{
		int64_t curmax = std::numeric_limits<KeyType>::min();
		int64_t curmin = std::numeric_limits<KeyType>::max();

		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<double> distr(0.0, 1.0);
		// fill array
		for (size_t i = 0; i < arr.size(); i++)
		{
			arr[i] = { static_cast<KeyType>(distr(eng)*((int64_t)max - min)	+ min), i };

#pragma warning( push )
#pragma warning( disable : 4018 )
			if (arr[i].first > curmax) curmax = arr[i].first;
			if (arr[i].first < curmin) curmin = arr[i].first;
#pragma warning( pop )
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

	template<typename KeyType>
	void TypeTest(KeyType min, KeyType max)
	{
		Array<KeyType> data(N);
		prepare_data<KeyType>(data, min, max);
		sort<KeyType>(data.begin(), data.end(), [](const auto& el) { return el.first; });
		check_data(data);
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
		TypeTest<KeyType>(std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max()/2);
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
		TypeTest<KeyType>(std::numeric_limits<KeyType>::min()/2, std::numeric_limits<KeyType>::max()/2);
	}
	
}}