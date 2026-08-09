#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "allradixsort/radixsort.hpp"

template<class KeyType>
using Array = std::vector<KeyType>;

template<class KeyType>
void prepare_data(Array<KeyType>& values, std::mt19937_64& engine)
{
	if constexpr (std::is_floating_point<KeyType>::value) {
		std::uniform_real_distribution<KeyType> distribution(KeyType{ -1000000 }, KeyType{ 1000000 });
		for (auto& value : values) {
			value = distribution(engine);
		}
	}
	else if constexpr (std::is_signed<KeyType>::value) {
		std::uniform_int_distribution<std::int64_t> distribution(
			static_cast<std::int64_t>(std::numeric_limits<KeyType>::min()),
			static_cast<std::int64_t>(std::numeric_limits<KeyType>::max()));
		for (auto& value : values) {
			value = static_cast<KeyType>(distribution(engine));
		}
	}
	else {
		std::uniform_int_distribution<std::uint64_t> distribution(
			static_cast<std::uint64_t>(std::numeric_limits<KeyType>::min()),
			static_cast<std::uint64_t>(std::numeric_limits<KeyType>::max()));
		for (auto& value : values) {
			value = static_cast<KeyType>(distribution(engine));
		}
	}
}

template<class KeyType>
void check_sort(const Array<KeyType>& values)
{
	if (!std::is_sorted(values.begin(), values.end())) {
		throw std::runtime_error("wrong sort order");
	}
}

template<class KeyType, class SortFn>
double run_sort_measure_time(const Array<KeyType>& input, std::size_t repeat, SortFn sort_fn)
{
	std::vector<Array<KeyType>> datasets(repeat, input);
	const auto start = std::chrono::steady_clock::now();
	for (auto& values : datasets) {
		sort_fn(values);
	}
	const auto stop = std::chrono::steady_clock::now();
	for (const auto& values : datasets) {
		check_sort(values);
	}
	return std::chrono::duration<double, std::milli>(stop - start).count();
}

template<class KeyType>
void run_performance_test(std::size_t size, std::size_t repeat, std::mt19937_64& engine)
{
	Array<KeyType> input(size);
	prepare_data(input, engine);

	const auto standard_duration = run_sort_measure_time(input, repeat, [](auto& values) {
		std::sort(values.begin(), values.end());
	});
	const auto radix_duration = run_sort_measure_time(input, repeat, [](auto& values) {
		allradixsort::sort(values.begin(), values.end());
	});

	std::cout << std::fixed << std::setprecision(2)
		<< "std::sort: " << standard_duration << " ms, radix sort: " << radix_duration << " ms";
	if (radix_duration > 0.0) {
		std::cout << ", speedup: " << standard_duration / radix_duration << "x";
	}
	std::cout << '\n';
}

template<class KeyType>
void run_named_test(const char* name, std::size_t size, std::size_t repeat, std::mt19937_64& engine)
{
	std::cout << name << " (" << size << " elements x " << repeat << ")\n";
	run_performance_test<KeyType>(size, repeat, engine);
}

int main()
{
	constexpr std::size_t num_elements = 1'000'000;
	constexpr std::size_t repeat = 10;
	std::mt19937_64 engine(0xA11BAD1ULL);

	run_named_test<std::uint8_t>("uint8_t", num_elements, repeat, engine);
	run_named_test<std::uint16_t>("uint16_t", num_elements, repeat, engine);
	run_named_test<std::uint32_t>("uint32_t", num_elements, repeat, engine);
	run_named_test<std::uint64_t>("uint64_t", num_elements, repeat, engine);
	run_named_test<std::int8_t>("int8_t", num_elements, repeat, engine);
	run_named_test<std::int16_t>("int16_t", num_elements, repeat, engine);
	run_named_test<std::int32_t>("int32_t", num_elements, repeat, engine);
	run_named_test<std::int64_t>("int64_t", num_elements, repeat, engine);
	run_named_test<float>("float", num_elements, repeat, engine);
	run_named_test<double>("double", num_elements, repeat, engine);
}
