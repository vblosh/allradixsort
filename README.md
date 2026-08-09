# allradixsort

`allradixsort` is a header-only C++17 stable LSD radix-sort library for these key types:

`uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`, `int8_t`, `int16_t`, `int32_t`, `int64_t`, `float`, and `double`.

The implementation is intended for large random-access ranges where its extra `O(n)` buffer is an acceptable tradeoff for linear-time sorting over a fixed number of radix passes.

## Usage

Sort primitive values with the `std::sort`-like interface:

```cpp
#include <vector>
#include <allradixsort/radixsort.hpp>

std::vector<std::int32_t> values{4, -1, 2, 2};
allradixsort::sort(values.begin(), values.end());
```

Sort records by returning or referencing a supported key:

```cpp
struct Item
{
    std::uint32_t id;
    float data;
};

std::vector<Item> items;
allradixsort::sort<std::uint32_t>(items.begin(), items.end(),
    [](const Item& item) { return item.id; });
```

Equal keys retain their original order. Iterators must be random-access. The value type must be default-constructible and move-assignable because the implementation uses a temporary `std::vector` buffer.

## Floating-point ordering

`float` and `double` require IEEE-754 representations. Their bit patterns are mapped to a deterministic total representation order:

- negative NaNs
- negative infinity and finite negative numbers
- `-0`, then `+0`
- finite positive numbers and positive infinity
- positive NaNs

This deliberately distinguishes signed zero and gives NaNs an order, unlike ordinary floating-point `<` comparisons. The implementation reads floating-point bits with `memcpy`, so it does not violate C++ strict-aliasing rules or modify keys while calculating radix digits.

## CMake

Use the repository directly:

```cmake
add_subdirectory(path/to/allradixsort)
target_link_libraries(your_target PRIVATE allradixsort::allradixsort)
```

The target propagates the C++17 requirement. Installation is also supported:

```console
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix /desired/prefix
```

An installed package can be consumed with `find_package(allradixsort CONFIG REQUIRED)` and the same `allradixsort::allradixsort` target.

## Tests and performance measurements

GoogleTest is discovered automatically for top-level builds. Run the tests with:

```console
ctest --test-dir build --output-on-failure
```

The optional performance executable compares `std::sort` and radix sort using the same deterministically generated inputs and validates every sorted copy:

```console
./build/perftests/perftests
```

Configure with `-DALLRADIXSORT_BUILD_TESTS=OFF` or `-DALLRADIXSORT_BUILD_BENCHMARKS=OFF` to disable either target. Performance depends on the compiler, optimization level, data type, distribution, hardware, and input size; benchmark locally rather than relying on a fixed speedup claim.

## License

MIT. See [LICENSE](LICENSE).
