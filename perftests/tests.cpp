#include <stdint.h>
#include <vector>
#include <random>
#include <limits>
#include <iostream>
#include <chrono>
#include <algorithm>

#include "radixsort.h"
#include <string>

//template<class KeyType>
//using Array = std::vector<KeyType>; 

//template<class KeyType>
//using Array = std::vector<std::pair<KeyType, std::string>>;

template<class KeyType>
struct Data
{
	KeyType first;
	std::string second;
};

template<class KeyType>
using Array = std::vector<Data<KeyType>>;

template<class KeyType>
void prepare_data(Array<KeyType>& arr, KeyType min, KeyType max)
{
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<double> distr(0.0, 1.0);
	// fill array
	for (size_t i = 0; i < arr.size(); i++)
	{
		arr[i] = { static_cast<KeyType>(distr(eng) * ((int64_t)max - min) + min), std::string("row number is ") + std::to_string(i) };
	}
}

template<>
void prepare_data<int64_t>(Array<int64_t>& arr, int64_t min, int64_t max)
{
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<double> distr(0.0, 1.0);
	// fill array
	for (size_t i = 0; i < arr.size(); i++)
	{
		arr[i] = { static_cast<int64_t>(distr(eng) * (max/10 - min/10) + min/10), std::string("row number is ") + std::to_string(i) };
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
			std::sort(data.begin(), data.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
		});
	std::cout << " std::sort " << size << " elements " << repeat << " times,  total " << size * repeat << " duration "
		<< duration1 << " ms\n";

	auto duration2 = RunSortMeasureTime(indata, repeat,
		[](Array<KeyType>& data)
		{
			allradixsort::sort<KeyType>(data.begin(), data.end(), [](auto& el)-> KeyType& { return el.first; });
		});
	std::cout << "radix sort " << size << " elements " << repeat << " times,  total " << size * repeat << " duration "
		<< duration2 << " ms\n";
	std::cout << "radix sort is " << double(duration1) / duration2 << "times faster for " << size << " elements\n";
}

int main()
{
	const size_t NUM_ELEM = 1000000;
	const size_t REPEAT = 10;

	std::cout << "Using uint8_t data\n";
	RunPerfomanceTest<uint8_t>(NUM_ELEM, REPEAT);
	std::cout << "Using uint16_t data\n";
	RunPerfomanceTest<uint16_t>(NUM_ELEM, REPEAT);
	std::cout << "Using uint32_t data\n";
	RunPerfomanceTest<uint32_t>(NUM_ELEM, REPEAT);
	std::cout << "Using uint64_t data\n";
	RunPerfomanceTest<uint64_t>(NUM_ELEM, REPEAT);
	std::cout << "Using int8_t data\n";
	RunPerfomanceTest<int8_t>(NUM_ELEM, REPEAT);
	std::cout << "Using int16_t data\n";
	RunPerfomanceTest<int16_t>(NUM_ELEM, REPEAT);
	std::cout << "Using int32_t data\n";
	RunPerfomanceTest<int32_t>(NUM_ELEM, REPEAT);
	std::cout << "Using int64_t data\n";
	RunPerfomanceTest<int64_t>(NUM_ELEM, REPEAT);
	std::cout << "Using float data\n";
	RunPerfomanceTest<float>(NUM_ELEM, REPEAT);
	std::cout << "Using double data\n";
	RunPerfomanceTest<double>(NUM_ELEM, REPEAT);

	return 0;
}

