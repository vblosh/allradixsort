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

constexpr size_t N = 10000000;

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

int main()
{
	using KeyType = uint8_t;
	Array<KeyType> data(N);
	
	{
		prepare_data<KeyType>(data, std::numeric_limits<KeyType>::min(), std::numeric_limits<KeyType>::max());
		auto start_time = std::chrono::high_resolution_clock::now();
		std::sort(data.begin(), data.end(),	[](const auto& a, const auto& b) { return a.first < b.first; });
		auto stop_time = std::chrono::high_resolution_clock::now();
		std::cout << "  std::sort " << N << " elements duration "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count() << " ms\n";
	}
	
	{
		auto start_time = std::chrono::high_resolution_clock::now();
		allradixsort::sort<KeyType>(data.begin(), data.end(),	[](const auto& el) { return el.first; });
		auto stop_time = std::chrono::high_resolution_clock::now();
		std::cout << "radix sort " << N << " elements duration "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count() << " ms\n";
	}

	return 0;
}

