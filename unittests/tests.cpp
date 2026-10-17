#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "allradixsort/floatsort.hpp"
#include "allradixsort/integersort.hpp"
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
			actual.emplace_back(std::numeric_limits<KeyType>::lowest(), 0);
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
			std::numeric_limits<KeyType>::lowest(), KeyType{ 1 }
		};
		auto expected = actual;
		std::stable_sort(expected.begin(), expected.end());
		allradixsort::sort(actual.begin(), actual.end());
		EXPECT_EQ(actual, expected);
	}

	struct DeduceRecord
	{
		int key;
		std::size_t payload;

		bool operator==(const DeduceRecord& other) const
		{
			return key == other.key && payload == other.payload;
		}
	};

	struct FloatDeduceRecord
	{
		double key;
		std::size_t payload;

		bool operator==(const FloatDeduceRecord& other) const
		{
			return key == other.key && payload == other.payload;
		}
	};
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

TEST(RadixSort, LargeInputSort)
{
	constexpr std::size_t count = 20000;
	std::mt19937_64 engine(42);

	// 1. std::uint32_t
	{
		std::vector<std::uint32_t> actual(count);
		for (auto& val : actual) {
			val = static_cast<std::uint32_t>(engine());
		}
		auto expected = actual;
		std::sort(expected.begin(), expected.end());
		allradixsort::sort(actual.begin(), actual.end());
		EXPECT_EQ(actual, expected);
	}

	// 2. std::int64_t
	{
		std::vector<std::int64_t> actual(count);
		for (auto& val : actual) {
			val = static_cast<std::int64_t>(engine());
		}
		auto expected = actual;
		std::sort(expected.begin(), expected.end());
		allradixsort::sort(actual.begin(), actual.end());
		EXPECT_EQ(actual, expected);
	}

	// 3. double
	{
		std::uniform_real_distribution<double> dist(-1e12, 1e12);
		std::vector<double> actual(count);
		for (auto& val : actual) {
			val = dist(engine);
		}
		actual[0] = std::numeric_limits<double>::infinity();
		actual[1] = -std::numeric_limits<double>::infinity();
		actual[2] = std::numeric_limits<double>::lowest();
		actual[3] = std::numeric_limits<double>::max();
		actual[4] = 0.0;
		actual[5] = -0.0;

		auto expected = actual;
		std::stable_sort(expected.begin(), expected.end(), [](double lhs, double rhs) {
			return float_key(lhs) < float_key(rhs);
		});
		allradixsort::sort(actual.begin(), actual.end());
		ASSERT_EQ(actual.size(), expected.size());
		for (std::size_t i = 0; i < actual.size(); ++i) {
			ASSERT_EQ(float_key(actual[i]), float_key(expected[i])) << "at index " << i;
		}
	}

	// 4. Record sorting with collisions and prefix sums
	{
		record_array<std::int64_t> actual;
		actual.reserve(count);
		for (std::size_t i = 0; i < count; ++i) {
			const auto key = static_cast<std::int64_t>(engine() % 500) - 250;
			actual.emplace_back(key, i);
		}
		auto expected = actual;
		std::stable_sort(expected.begin(), expected.end(),
			[](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
		allradixsort::sort<std::int64_t>(actual.begin(), actual.end(),
			[](const auto& r) { return r.first; });
		EXPECT_EQ(actual, expected);
	}
}

TEST(RadixSort, FundamentalIntegerTypes)
{
	std::mt19937_64 engine(12345);

	auto test_type = [&engine](auto dummy) {
		using T = decltype(dummy);
		check_scalar_sort<T>();

		std::vector<T> actual(500);
		for (auto& val : actual) {
			val = static_cast<T>(engine());
		}
		auto expected = actual;
		std::sort(expected.begin(), expected.end());
		allradixsort::sort(actual.begin(), actual.end());
		EXPECT_EQ(actual, expected);
	};

	test_type(char{});
	test_type(static_cast<signed char>(0));
	test_type(static_cast<unsigned char>(0));
	test_type(short{});
	test_type(static_cast<unsigned short>(0));
	test_type(int{});
	test_type(static_cast<unsigned int>(0));
	test_type(long{});
	test_type(static_cast<unsigned long>(0));
	test_type(static_cast<long long>(0));
	test_type(static_cast<unsigned long long>(0));
}

TEST(RadixSort, KeyTypeDeduction)
{
	std::mt19937_64 engine(777);

	// Integer key deduction
	{
		std::vector<DeduceRecord> actual;
		for (std::size_t i = 0; i < 1000; ++i) {
			actual.push_back({ static_cast<int>(engine() % 200) - 100, i });
		}
		const auto initial = actual;
		auto expected = initial;
		std::stable_sort(expected.begin(), expected.end(),
			[](const auto& lhs, const auto& rhs) { return lhs.key < rhs.key; });

		allradixsort::sort(actual.begin(), actual.end(),
			[](const auto& r) { return r.key; });
		EXPECT_EQ(actual, expected);

		// Pointer-to-member projection via std::invoke
		auto actual_member = initial;
		allradixsort::sort(actual_member.begin(), actual_member.end(), &DeduceRecord::key);
		EXPECT_EQ(actual_member, expected);
	}

	// Constexpr key normalization
	{
		constexpr auto k_neg = allradixsort::normalized_integer_key(std::int32_t(-1));
		constexpr auto k_zero = allradixsort::normalized_integer_key(std::int32_t(0));
		constexpr auto k_pos = allradixsort::normalized_integer_key(std::int32_t(1));
		static_assert(k_neg < k_zero && k_zero < k_pos, "normalized_integer_key must be constexpr");
	}

	// Floating point key deduction
	{
		std::uniform_real_distribution<double> dist(-500.0, 500.0);
		std::vector<FloatDeduceRecord> actual;
		for (std::size_t i = 0; i < 1000; ++i) {
			actual.push_back({ dist(engine), i });
		}
		auto expected = actual;
		std::stable_sort(expected.begin(), expected.end(),
			[](const auto& lhs, const auto& rhs) { return float_key(lhs.key) < float_key(rhs.key); });

		allradixsort::sort(actual.begin(), actual.end(),
			[](const auto& r) { return r.key; });
		for (std::size_t i = 0; i < actual.size(); ++i) {
			ASSERT_EQ(float_key(actual[i].key), float_key(expected[i].key));
			ASSERT_EQ(actual[i].payload, expected[i].payload);
		}
	}
}

TEST(RadixSort, NarrowRangeAndIdenticalElements)
{
	// 1. All elements identical (all passes skipped)
	{
		std::vector<std::pair<int, std::size_t>> actual;
		constexpr std::size_t count = 1000;
		for (std::size_t i = 0; i < count; ++i) {
			actual.emplace_back(42, i);
		}
		auto expected = actual;
		allradixsort::sort(actual.begin(), actual.end(), [](const auto& p) { return p.first; });
		EXPECT_EQ(actual, expected);
	}

	// 2. Narrow range in low byte for uint32_t (passes 1, 2, 3 skipped)
	{
		std::vector<std::pair<std::uint32_t, std::size_t>> actual;
		for (std::size_t i = 0; i < 1000; ++i) {
			const auto key = 0x12345600U | static_cast<std::uint32_t>((i * 37) % 256);
			actual.emplace_back(key, i);
		}
		auto expected = actual;
		std::stable_sort(expected.begin(), expected.end(),
			[](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
		allradixsort::sort(actual.begin(), actual.end(), [](const auto& p) { return p.first; });
		EXPECT_EQ(actual, expected);
	}

	// 3. Narrow range in high byte for uint32_t (passes 0, 1, 2 skipped)
	{
		std::vector<std::pair<std::uint32_t, std::size_t>> actual;
		for (std::size_t i = 0; i < 1000; ++i) {
			const auto key = static_cast<std::uint32_t>((i * 17) % 256) << 24;
			actual.emplace_back(key, i);
		}
		auto expected = actual;
		std::stable_sort(expected.begin(), expected.end(),
			[](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
		allradixsort::sort(actual.begin(), actual.end(), [](const auto& p) { return p.first; });
		EXPECT_EQ(actual, expected);
	}

	// 4. Narrow range with signed 64-bit integers (negative numbers sharing sign/upper bits)
	{
		std::vector<std::pair<std::int64_t, std::size_t>> actual;
		for (std::size_t i = 0; i < 1000; ++i) {
			const auto key = -1000000000LL + static_cast<std::int64_t>((i * 13) % 100);
			actual.emplace_back(key, i);
		}
		auto expected = actual;
		std::stable_sort(expected.begin(), expected.end(),
			[](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
		allradixsort::sort(actual.begin(), actual.end(), [](const auto& p) { return p.first; });
		EXPECT_EQ(actual, expected);
	}

	// 5. Identical floating point elements
	{
		std::vector<std::pair<double, std::size_t>> actual;
		for (std::size_t i = 0; i < 500; ++i) {
			actual.emplace_back(3.141592653589793, i);
		}
		auto expected = actual;
		allradixsort::sort(actual.begin(), actual.end(), [](const auto& p) { return p.first; });
		EXPECT_EQ(actual, expected);
	}

	// 6. Narrow range floating point elements (sharing upper exponent/mantissa bytes)
	{
		std::vector<std::pair<double, std::size_t>> actual;
		for (std::size_t i = 0; i < 500; ++i) {
			actual.emplace_back(1000.0 + static_cast<double>((i * 7) % 50) * 0.001, i);
		}
		auto expected = actual;
		std::stable_sort(expected.begin(), expected.end(),
			[](const auto& lhs, const auto& rhs) { return float_key(lhs.first) < float_key(rhs.first); });
		allradixsort::sort(actual.begin(), actual.end(), [](const auto& p) { return p.first; });
		for (std::size_t i = 0; i < actual.size(); ++i) {
			EXPECT_EQ(float_key(actual[i].first), float_key(expected[i].first));
			EXPECT_EQ(actual[i].second, expected[i].second);
		}
	}
}

TEST(RadixSort, SubsortInterfaces)
{
	// 1. integer_sort 2-argument overload
	{
		std::vector<int> actual{ 42, -5, 0, 100, -999, 17, 3 };
		auto expected = actual;
		std::sort(expected.begin(), expected.end());
		allradixsort::integer_sort(actual.begin(), actual.end());
		EXPECT_EQ(actual, expected);

		// With explicit KeyType
		allradixsort::integer_sort<int>(actual.begin(), actual.end());
		EXPECT_EQ(actual, expected);
	}

	// 2. integer_sort 3-argument overload
	{
		std::vector<std::pair<int, char>> actual{
			{ 5, 'a' }, { -1, 'b' }, { 10, 'c' }, { -1, 'd' }, { 0, 'e' }
		};
		auto expected = actual;
		std::stable_sort(expected.begin(), expected.end(),
			[](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });

		// Deduced KeyType
		allradixsort::integer_sort(actual.begin(), actual.end(),
			[](const auto& p) { return p.first; });
		EXPECT_EQ(actual, expected);

		// Explicit KeyType
		actual = { { 5, 'a' }, { -1, 'b' }, { 10, 'c' }, { -1, 'd' }, { 0, 'e' } };
		allradixsort::integer_sort<int>(actual.begin(), actual.end(),
			[](const auto& p) { return p.first; });
		EXPECT_EQ(actual, expected);
	}

	// 3. float_sort 2-argument overload
	{
		std::vector<float> actual{ 3.14f, -0.5f, 0.0f, 100.25f, -999.0f, 17.125f };
		auto expected = actual;
		std::sort(expected.begin(), expected.end());
		allradixsort::float_sort(actual.begin(), actual.end());
		EXPECT_EQ(actual, expected);

		// With explicit KeyType
		allradixsort::float_sort<float>(actual.begin(), actual.end());
		EXPECT_EQ(actual, expected);
	}

	// 4. float_sort 3-argument overload
	{
		std::vector<std::pair<double, char>> actual{
			{ 5.5, 'a' }, { -1.2, 'b' }, { 10.1, 'c' }, { -1.2, 'd' }, { 0.0, 'e' }
		};
		auto expected = actual;
		std::stable_sort(expected.begin(), expected.end(),
			[](const auto& lhs, const auto& rhs) { return float_key(lhs.first) < float_key(rhs.first); });

		// Deduced KeyType
		allradixsort::float_sort(actual.begin(), actual.end(),
			[](const auto& p) { return p.first; });
		for (std::size_t i = 0; i < actual.size(); ++i) {
			EXPECT_EQ(float_key(actual[i].first), float_key(expected[i].first));
			EXPECT_EQ(actual[i].second, expected[i].second);
		}

		// Explicit KeyType
		actual = { { 5.5, 'a' }, { -1.2, 'b' }, { 10.1, 'c' }, { -1.2, 'd' }, { 0.0, 'e' } };
		allradixsort::float_sort<double>(actual.begin(), actual.end(),
			[](const auto& p) { return p.first; });
		for (std::size_t i = 0; i < actual.size(); ++i) {
			EXPECT_EQ(float_key(actual[i].first), float_key(expected[i].first));
			EXPECT_EQ(actual[i].second, expected[i].second);
		}
	}
}
