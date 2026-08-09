#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "allradixsort/radixsort.hpp"

namespace
{
	template<class KeyType>
	using record_array = std::vector<std::pair<KeyType, std::size_t>>;

	template<class KeyType>
	void check_integral_records(std::uint64_t seed)
	{
		std::mt19937_64 engine(seed);
		for (std::size_t round = 0; round < 100; ++round) {
			record_array<KeyType> actual;
			const auto count = static_cast<std::size_t>(engine() % 400);
			actual.reserve(count + 5);
			actual.emplace_back(std::numeric_limits<KeyType>::min(), 0);
			actual.emplace_back(std::numeric_limits<KeyType>::max(), 1);
			actual.emplace_back(KeyType{ 0 }, 2);
			actual.emplace_back(KeyType{ 0 }, 3);
			actual.emplace_back(KeyType{ 1 }, 4);
			for (std::size_t index = 0; index < count; ++index) {
				actual.emplace_back(static_cast<KeyType>(engine()), index + 5);
			}

			auto expected = actual;
			std::stable_sort(expected.begin(), expected.end(),
				[](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
			allradixsort::sort<KeyType>(actual.begin(), actual.end(),
				[](const auto& element) { return element.first; });
			ASSERT_EQ(actual, expected) << "round " << round;
		}
	}

	template<class KeyType>
	auto float_key(KeyType value)
	{
		using proxy_type = typename allradixsort::traits<KeyType>::proxy_type;
		return allradixsort::float_flip<KeyType, proxy_type>(value);
	}

	template<class KeyType>
	void check_float_records(std::uint64_t seed)
	{
		std::mt19937_64 engine(seed);
		std::uniform_real_distribution<KeyType> distribution(KeyType{ -100000 }, KeyType{ 100000 });
		for (std::size_t round = 0; round < 100; ++round) {
			record_array<KeyType> actual{
				{ -std::numeric_limits<KeyType>::quiet_NaN(), 0 },
				{ -std::numeric_limits<KeyType>::infinity(), 1 },
				{ -KeyType{ 0 }, 2 },
				{ KeyType{ 0 }, 3 },
				{ std::numeric_limits<KeyType>::denorm_min(), 4 },
				{ std::numeric_limits<KeyType>::infinity(), 5 },
				{ std::numeric_limits<KeyType>::quiet_NaN(), 6 }
			};
			const auto count = static_cast<std::size_t>(engine() % 400);
			actual.reserve(count + actual.size());
			for (std::size_t index = 0; index < count; ++index) {
				actual.emplace_back(distribution(engine), index + 7);
			}

			auto expected = actual;
			std::stable_sort(expected.begin(), expected.end(), [](const auto& lhs, const auto& rhs) {
				return float_key(lhs.first) < float_key(rhs.first);
			});
			allradixsort::sort<KeyType>(actual.begin(), actual.end(),
				[](const auto& element) { return element.first; });

			ASSERT_EQ(actual.size(), expected.size());
			for (std::size_t index = 0; index < actual.size(); ++index) {
				ASSERT_EQ(float_key(actual[index].first), float_key(expected[index].first))
					<< "round " << round << ", index " << index;
				ASSERT_EQ(actual[index].second, expected[index].second)
					<< "round " << round << ", index " << index;
			}
		}
	}

	template<class KeyType>
	void check_scalar_sort()
	{
		std::vector<KeyType> actual{
			std::numeric_limits<KeyType>::max(), KeyType{ 1 }, KeyType{ 0 },
			std::numeric_limits<KeyType>::min(), KeyType{ 1 }
		};
		auto expected = actual;
		std::stable_sort(expected.begin(), expected.end());
		allradixsort::sort(actual.begin(), actual.end());
		EXPECT_EQ(actual, expected);
	}
}

int allradixsort_odr_a();
int allradixsort_odr_b();

TEST(RadixSort, HeaderWorksAcrossTranslationUnits)
{
	EXPECT_EQ(allradixsort_odr_a() + allradixsort_odr_b(), 3);
}

TEST(RadixSort, StableIntegralRecordSort)
{
	check_integral_records<std::uint8_t>(1);
	check_integral_records<std::uint16_t>(2);
	check_integral_records<std::uint32_t>(3);
	check_integral_records<std::uint64_t>(4);
	check_integral_records<std::int8_t>(5);
	check_integral_records<std::int16_t>(6);
	check_integral_records<std::int32_t>(7);
	check_integral_records<std::int64_t>(8);
}

TEST(RadixSort, TotalFloatRecordSort)
{
	check_float_records<float>(9);
	check_float_records<double>(10);
}

TEST(RadixSort, ScalarInterface)
{
	check_scalar_sort<std::uint8_t>();
	check_scalar_sort<std::uint16_t>();
	check_scalar_sort<std::uint32_t>();
	check_scalar_sort<std::uint64_t>();
	check_scalar_sort<std::int8_t>();
	check_scalar_sort<std::int16_t>();
	check_scalar_sort<std::int32_t>();
	check_scalar_sort<std::int64_t>();
	check_scalar_sort<float>();
	check_scalar_sort<double>();
}

TEST(RadixSort, EmptyAndSingleElementRanges)
{
	std::vector<std::int32_t> empty;
	allradixsort::sort(empty.begin(), empty.end());
	EXPECT_TRUE(empty.empty());

	std::vector<double> single{ 42.0 };
	allradixsort::sort(single.begin(), single.end());
	ASSERT_EQ(single.size(), 1U);
	EXPECT_EQ(single.front(), 42.0);
}
