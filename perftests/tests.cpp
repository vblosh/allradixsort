#include <stdint.h>
#include <vector>
#include <random>
#include <limits>
#include <iostream>
#include <chrono>
#include <algorithm>

#include "radixsort.h"

template<class KeyType>
using Array = std::vector<KeyType>; 

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
		arr[i] = { static_cast<KeyType>(distr(eng) * ((int64_t)max - min) + min) };
	}
}

template<class KeyType, class SortFn>
long long RunSortMeasureTime(Array<KeyType>& indata, size_t repeat, SortFn sort_fn)
{
	std::vector<Array<KeyType>> data(repeat, indata);
	auto start_time = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < repeat; i++)
	{
		sort_fn(data[i]);
	}
	auto stop_time = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
}

template<class KeyType>
void RunPerfomanceTest(size_t size, size_t repeat)
{
	Array<KeyType> indata(size);
	prepare_data<KeyType>(indata, std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());

	auto duration1 = RunSortMeasureTime(indata, repeat,
		[](Array<KeyType>& data)
		{
			std::sort(data.begin(), data.end());
		});
	std::cout << " std::sort " << size << " elements " << repeat << " times,  total " << size * repeat << " duration "
		<< duration1 << " ms\n";

	auto duration2 = RunSortMeasureTime(indata, repeat,
		[](Array<KeyType>& data)
		{
			allradixsort::sort(data.begin(), data.end());
		});
	std::cout << "radix sort " << size << " elements " << repeat << " times,  total " << size * repeat << " duration "
		<< duration2 << " ms\n";
	std::cout << "radix sort is " << double(duration1) / duration2 * 100 << "% faster for " << size << " elements\n";
}

int main()
{
	RunPerfomanceTest<uint32_t>(1000000, 10);
	RunPerfomanceTest<uint32_t>(10000000, 1);

	return 0;
}

