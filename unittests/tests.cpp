#include <stdint.h>
#include <gtest/gtest.h>
#include <vector>
#include <array>
#include <functional>
#include "radixsort.h"


using KeyTyp = unsigned short;
using ValueType = size_t;
using Typ = std::pair<KeyTyp, ValueType>;
using Array = std::vector<Typ>;

constexpr size_t N = 1000000;

using namespace std;
namespace allradixsort
{

	void prepare_data(Array& arr, size_t MAX)
	{
		srand(1);
		// fill array
		for (size_t i = 0; i < arr.size(); i++)
		{
			arr[i] = { static_cast<KeyTyp>(MAX * rand() / RAND_MAX), i };
		}
	}

	bool check_data(Array& InArr)
	{
		bool res = true;
		for (size_t i = 1; i < InArr.size(); ++i)
		{
			if (InArr[i - 1].first > InArr[i].first
				|| (InArr[i - 1].first == InArr[i].first && InArr[i - 1].second > InArr[i].second)) {
				res = false;
				break;
			}
		}
		return res;
	}

	TEST(CountingSort, uint8_t_test)
	{
		const size_t MAX = 255;
		Array data(N);
		prepare_data(data, MAX);
		auto out = counting_sort(data.cbegin(), data.cend(),
			[](const auto& el) { return el.first; }
			, MAX);
		ASSERT_TRUE(check_data(out));
	}
}