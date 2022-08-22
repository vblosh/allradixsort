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
		KeyType curmax = std::numeric_limits<KeyType>::min();
		KeyType curmin = std::numeric_limits<KeyType>::max();

		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<double> distr(0.0, 1.0);
		// fill array
		for (size_t i = 0; i < arr.size(); i++)
		{
			arr[i] = { static_cast<KeyType>(distr(eng)*((int64_t)max - min)	+ min), i };

			if (arr[i].first > curmax) curmax = arr[i].first;
			if (arr[i].first < curmin) curmin = arr[i].first;
		}
		//std::cout << " min=" << curmin << " max=" << curmax  << '\n';
	}

	template<>
	void prepare_data<float>(Array<float>& arr, float min, float max)
	{
		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<float> distr(min, max);
		// fill array
		for (size_t i = 0; i < arr.size(); i++)
		{
			arr[i] = { distr(eng), i };
		}
	}

	template<>
	void prepare_data<double>(Array<double>& arr, double min, double max)
	{
		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<double> distr(min, max);
		// fill array
		for (size_t i = 0; i < arr.size(); i++)
		{
			arr[i] = { distr(eng), i };
		}
	}

	template<class KeyType>
	void check_sort(Array<KeyType>& InArr)
	{
		for (size_t i = 1; i < InArr.size(); ++i)
		{
			ASSERT_FALSE(InArr[i - 1].first > InArr[i].first
				|| (InArr[i - 1].first == InArr[i].first 
					&& InArr[i - 1].second > InArr[i].second));
		}
	}

	template<class KeyType>
	void check_same(Array<KeyType>& InArr, Array<KeyType>& InArrCopy)
	{
		std::sort(InArrCopy.begin(), InArrCopy.end());
		for (size_t i = 0; i < InArr.size(); ++i)
		{
			ASSERT_TRUE(InArrCopy[i].first == InArr[i].first);
		}
	}

	template<typename KeyType>
	void TypeTest(KeyType min, KeyType max)
	{
		Array<KeyType> data(N);
		prepare_data<KeyType>(data, min, max);
		Array<KeyType> data_copy(data);

		sort<KeyType>(data.begin(), data.end(), [](auto& el) -> KeyType& { return el.first; });
		// check sorting
		check_sort(data);
		// check if data is the same
		check_same(data, data_copy);
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

	TEST(RadixSort, float_test)
	{
		using KeyType = float;
		TypeTest<KeyType>(-1000.0, 1000.0);
	}
	
	TEST(RadixSort, double_test)
	{
		using KeyType = double;
		TypeTest<KeyType>(-1000.0, 1000.0);
	}
}}