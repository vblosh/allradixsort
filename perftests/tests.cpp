#include <stdint.h>
#include <vector>
#include <random>
#include <limits>
#include <iostream>
#include <chrono>
#include <algorithm>

#include "radixsort.h"

template<class KeyType>
using Array = std::vector<std::pair<KeyType, KeyType>>;

constexpr size_t N = 1000000;
const size_t REPEAT = 10;

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
		arr[i] = { static_cast<KeyType>(distr(eng)*((int64_t)max - min)	+ min)
			, static_cast<KeyType>(i) };
		if (arr[i].first > curmax) curmax = arr[i].first;
		if (arr[i].first < curmin) curmin = arr[i].first;
	}
	//std::cout << " min=" << curmin << " max=" << curmax  << '\n';
}

template<class KeyType, class SortFn>
long long RunSortMeasureTime(Array<KeyType>& indata, SortFn sort_fn)
{
	std::vector<Array<KeyType>> data(REPEAT, indata);
	auto start_time = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < REPEAT; i++)
	{
		sort_fn(data[i]);
	}
	auto stop_time = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
}

int main()
{
	using KeyType = uint32_t;
	Array<KeyType> indata(N);
	prepare_data<KeyType>(indata, std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());
	
	long long duration;
	duration = RunSortMeasureTime(indata,
		[](Array<KeyType>& data) 
		{ 
			std::sort(data.begin(), data.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
		});
	std::cout << " std::sort " << N << " elements " << REPEAT << " times,  total " << N * REPEAT << " duration "
		<< duration << " ms\n";

	duration = RunSortMeasureTime(indata,
		[](Array<KeyType>& data)
		{
			allradixsort::sort<KeyType>(data.begin(), data.end(), [](const auto& el) { return el.first; });
		});
	std::cout << "radix sort " << N << " elements " << REPEAT << " times,  total " << N * REPEAT << " duration "
		<< duration << " ms\n";

	return 0;
}

